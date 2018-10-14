#define	KERNEL_SIZE				9
#define MAX_REFLEXION_STEPS		5
#define MAX_REFLEXION_SAMPLES	4

uniform vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(0.95581, -0.18159), vec2(0.50147, -0.35807), vec2(0.69607, 0.35559),
	vec2(-0.0036825, -0.59150),	vec2(0.15930, 0.089750), vec2(-0.65031, 0.058189),
	vec2(0.11915, 0.78449),	vec2(-0.34296, 0.51575), vec2(-0.60380, -0.41527));

uniform vec3		brightnessDotValue = vec3(0.299, 0.587, 0.114);
uniform vec3		envGammaCorrection = vec3(2.2);
uniform sampler2D	LastColor;
uniform sampler2D	LastEmitting;
uniform sampler2D	LastDepth;

vec3	UVFromPosition(in vec3 position)
{
	vec4	projectedPosition = Camera.Matrix.Projection * Camera.Matrix.View * vec4(position, 1);
	projectedPosition /= projectedPosition.w;
	projectedPosition = projectedPosition * 0.5 + 0.5;
	return (projectedPosition.xyz);
}

vec4	SSR()
{
	vec3	V = normalize(Frag.Position - Camera.Position);
	vec3	R = reflect(V, Frag.Normal);
	float	curLength = 0.25;
	vec4	ret = vec4(0);
	float	hits = 0;

	for (uint i = 0; i < MAX_REFLEXION_STEPS; i++)
	{
		vec3	curPos = R * curLength + Frag.Position; //Calculate current step's position
		vec3	curUV = UVFromPosition(curPos); //Compute step's screen coordinates
		vec2	sampleUV = curUV.xy;
		float	sampleDepth;
		for (uint j = 0; j < MAX_REFLEXION_SAMPLES; j++)
		{
			sampleDepth = texture(LastDepth, sampleUV.xy).r; //Get precise depth value at pixel
			//Don't check if sampleUV is offscreen, this would result in even more branching code and hurt performances
			//If current step behind ZBuffer or at "almost" same depth, accept as valid reflexion (avoids holes)
			if (abs(curUV.z - sampleDepth) <= 0.1 || curUV.z >= sampleDepth)
			{
				ret.xyz += sampleLod(LastColor, sampleUV.xy, Frag.Material.Roughness * 2).rgb; //Sample last image color and accumulate it
				ret.xyz += sampleLod(LastEmitting, sampleUV.xy, Frag.Material.Roughness).rgb; //LastEmitting is already blurred
				float	screenEdgeFactor = 1;
				screenEdgeFactor -= smoothstep(0, 1, pow(abs(sampleUV.x * 2 - 1), 5.f)); //Attenuate reflection factor when getting closer to screen border
				screenEdgeFactor -= smoothstep(0, 1, pow(abs(sampleUV.y * 2 - 1), 5.f));
				ret.w += clamp(screenEdgeFactor, 0, 1);
				hits++;
			}
			sampleUV = curUV.xy + poissonDisk[j] * (0.001 * Frag.Material.Roughness + 0.0001); //Offset sampling to look around a bit
		}
		curLength = length(Frag.Position - Position(curUV.xy, sampleDepth)); //Advance in ray marching proportionaly to current point's distance (make sure you don't miss anything)
	}
	if (hits > 0) {
		ret /= hits; //Compute average color and attenuation
		ret.w *= dot(R, V);
	}
	return (ret);
}

float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	den = (alpha - 1) + 1;
	float	D = (alpha / (M_PI * den * den));
	float	alpha2 = alpha * alpha;
	float	G = (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
	return (D * G);
}

vec3	Fresnel(in float factor, in vec3 F0, in float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
}

void	ApplyTechnique()
{
	const vec3	EnvDiffuse = texture(Texture.Environment.Diffuse, Frag.CubeUV).rgb;

	Frag.Material.AO = 1 - Frag.Material.AO;
	
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	N = Frag.Normal;
#ifdef TRANSPARENT
	float	NdV = dot(N, V);
	if (Frag.Material.Alpha < 1 && NdV < 0) {
		N = -N;
		NdV = -NdV;
	}
	else {
		NdV = max(0, dot(N, V));
	}
#else
	float	NdV = max(0, dot(N, V));
#endif //TRANSPARENT
	vec3	R = reflect(V, N);

	const vec2	BRDF = BRDF(NdV, Frag.Material.Roughness);

	vec3	diffuse = Frag.Material.AO * (sampleLod(Texture.Environment.Diffuse, -N, Frag.Material.Roughness + 0.9).rgb
			+ texture(Texture.Environment.Irradiance, -N).rgb);
	vec3	reflection = sampleLod(Texture.Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = texture(Texture.Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;


	float	brightness = 0;

	if (Frag.Material.Alpha == 0) {
	#ifdef TRANSPARENT
		return ;
	#endif //TRANSPARENT
		Out.Color = vec4(EnvDiffuse, 1);
		brightness = dot(pow(Out.Color.rgb, envGammaCorrection), brightnessDotValue);
		Out.Emitting = max(vec3(0), (Out.Color.rgb - 0.8) * min(1, brightness));
		return ;
	}
	vec3	fresnel = min(vec3(0.5), Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness));
	reflection *= fresnel;
	brightness = dot(pow(reflection_spec, envGammaCorrection), brightnessDotValue);
	reflection_spec *= brightness * min(fresnel + 0.5, fresnel * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= fresnel * BRDF.x + mix(vec3(1), fresnel, Frag.Material.Metallic) * BRDF.y;
	diffuse *= Frag.Material.Albedo.rgb * (1 - Frag.Material.Metallic);

	float	alpha = Frag.Material.Alpha + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);

	vec3	envReflection = (specular + reflection_spec + reflection) * alpha;

	vec4	ssrResult = vec4(0);
	if (Frag.Material.Roughness < 1) {
		ssrResult = SSR();
	}
	if (ssrResult.w > 0) {
		Out.Color.rgb += mix(envReflection, ssrResult.xyz * fresnel, clamp(ssrResult.w, 0, 1));
	}
	else {
		Out.Color.rgb += envReflection;
	}
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}
