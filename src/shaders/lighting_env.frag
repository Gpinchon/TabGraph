R""(
#define	KERNEL_SIZE				9

const vec2 poissonDisk[] = vec2[KERNEL_SIZE](
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

/*
vec2	UVSamplingAttenuation = smoothstep(vec2(0.05), vec2(0.1), sampleUV.xy) * (1 - smoothstep(vec2(0.95),vec2 (1), sampleUV.xy));
UVSamplingAttenuation.x *= UVSamplingAttenuation.y;
if (UVSamplingAttenuation.x > 0)
{
	UVSamplingAttenuation.x = clamp(UVSamplingAttenuation.x, 0, 1);
	ret.xyz += sampleLod(LastColor, sampleUV.xy, Frag.Material.Roughness * 2).rgb * UVSamplingAttenuation.x; //Sample last image color and accumulate it
	ret.xyz += sampleLod(LastEmitting, sampleUV.xy, Frag.Material.Roughness).rgb * UVSamplingAttenuation.x; //LastEmitting is already blurred
	ret.w += UVSamplingAttenuation.x;
}
*/

vec4	SSR()
{
	vec3	V = normalize(Frag.Position - Camera.Position);
	vec3	R = reflect(V, Frag.Normal);
	bool	intersected[REFLEXION_SAMPLES];
	vec2	RSPoisson[REFLEXION_SAMPLES];

	for (uint i = 0; i < REFLEXION_SAMPLES; i++) {
		RSPoisson[i] = poissonDisk[i] * (Frag.Material.Roughness + 0.0001);
		intersected[i] = false;
	}
	
	/*
	vec3	RSDirs[REFLEXION_SAMPLES];
	float	sampleAngle = randomAngle(Frag.Position, 1024);
	vec2	sampleRotation = vec2(cos(sampleAngle), -sin(sampleAngle));
	for (uint i = 0; i < REFLEXION_SAMPLES; i++) {
		vec2 offset = poissonDisk[i] * sampleRotation * (Frag.Material.Roughness + 0.0001);
		RSDirs[i] = R + vec3(offset.x, offset.y, offset.x * offset.y);
		intersected[i] = false;
	} 
	*/
	float	curLength = 0.5;
	vec4	ret = vec4(0);
	float	hits = 0;
	float	CameraFacingReflectionAttenuation = dot(R, V);//1 - smoothstep(0.25, 0.5, dot(-V, R));
	if (CameraFacingReflectionAttenuation <= 0)
		return (vec4(0));
	for (uint i = 0; i < REFLEXION_STEPS; i++)
	{
		vec3	curPos = R * curLength + Frag.Position;
		vec3	curUV = UVFromPosition(curPos); //Compute step's screen coordinates
		if (curUV.z >= 1)
			break ;
		float	sampleAngle = randomAngle(curPos, 1024);
		vec2	sampleRotation = vec2(cos(sampleAngle), -sin(sampleAngle));
		bool	allRaysIntersected = true;
		for (uint j = 0; j < REFLEXION_SAMPLES; j++)
		{
			vec2	offset = RSPoisson[j] * sampleRotation;
			vec3	RSDir = R + vec3(offset.x, offset.y, offset.x * offset.y);
			vec3	sampleUV = UVFromPosition(RSDir * curLength + Frag.Position);
			if (sampleUV.z >= 1)
				break;
			//if (intersected[j])
			//	continue;
			float sampleDepth = texture(LastDepth, sampleUV.xy).r;
			if (abs(sampleUV.z - sampleDepth) <= 0.01)
			{
				float	screenEdgeFactor = 1;
				//vec2	UVSamplingAttenuation = smoothstep(0.05, 0.1, sampleUV.xy) * (1 - smoothstep(0.95, 1.0, sampleUV.xy));
				//screenEdgeFactor = UVSamplingAttenuation.x *= UVSamplingAttenuation.y;
				screenEdgeFactor -= smoothstep(0, 1, pow(abs(sampleUV.x * 2 - 1), SCREEN_BORDER_FACTOR)); //Attenuate reflection factor when getting closer to screen border
				screenEdgeFactor -= smoothstep(0, 1, pow(abs(sampleUV.y * 2 - 1), SCREEN_BORDER_FACTOR));
				if (screenEdgeFactor > 0)
				{
					hits++;
					intersected[j] = true;
					allRaysIntersected = allRaysIntersected && intersected[j];
					screenEdgeFactor = clamp(screenEdgeFactor, 0, 1);
					ret.xyz += sampleLod(LastColor, sampleUV.xy, Frag.Material.Roughness * 2).rgb * screenEdgeFactor; //Sample last image color and accumulate it
					ret.xyz += sampleLod(LastEmitting, sampleUV.xy, Frag.Material.Roughness).rgb * screenEdgeFactor; //LastEmitting is already blurred
					ret.w += screenEdgeFactor;
				}
			}
		}
		//if (allRaysIntersected)
		//	break;
		float	curDepth = texture(LastDepth, curUV.xy).r;
		curLength = length(Frag.Position - Position(curUV.xy, curDepth)); //Advance in ray marching proportionaly to current point's distance (make sure you don't miss anything)
	}
	if (hits > 0) {
		ret /= hits; //Compute average color and attenuation
		ret.w *= CameraFacingReflectionAttenuation;
	}
	return (ret);
}

float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	alpha2 = alpha * alpha;
	float	den = (alpha2 - 1) + 1;
	float	D = alpha2 / (M_PI * den * den);
	float	k = alpha / 2.f;
	float	denom = NdV * (1.f - k) + k;
	float	G = NdV / denom;
	return (max(D * G, 0));
}

vec3	Fresnel(in float factor, in vec3 F0, in float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
}

void	ApplyTechnique()
{
	vec3	EnvDiffuse = texture(Texture.Environment.Diffuse, Frag.CubeUV).rgb;

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

	vec2	brdf = BRDF(NdV, Frag.Material.Roughness);

	vec3	diffuse = Frag.Material.AO * (sampleLod(Texture.Environment.Diffuse, -N, Frag.Material.Roughness + 0.9).rgb
			+ texture(Texture.Environment.Irradiance, -N).rgb);
	vec3	reflection = sampleLod(Texture.Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = texture(Texture.Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;

	float	brightness = 0;

	if (Frag.Material.Alpha == 0) {
	#ifdef TRANSPARENT
		return ;
	#else
		Out.Color = vec4(EnvDiffuse, 1);
		brightness = dot(pow(Out.Color.rgb, envGammaCorrection), brightnessDotValue);
		Out.Emitting = max(vec3(0), (Out.Color.rgb - 0.8) * min(1, brightness));
		return ;
	#endif //TRANSPARENT
	}
	vec3	fresnel = min(vec3(0.5), Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness));
	reflection *= fresnel;
	brightness = dot(pow(reflection_spec, envGammaCorrection), brightnessDotValue);
	reflection_spec *= brightness * min(fresnel + 0.5, fresnel * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= fresnel * brdf.x + mix(vec3(1), fresnel, Frag.Material.Metallic) * brdf.y;
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

)""