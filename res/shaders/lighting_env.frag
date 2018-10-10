uniform vec3	brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
uniform vec3	envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

uniform sampler2D	LastColor;
uniform sampler2D	LastEmitting;
uniform sampler2D	LastDepth;

#define MAX_REFLEXION_STEPS	15

vec3	UVFromPosition(in vec3 position)
{
	vec4	projectedPosition = Camera.Matrix.Projection * Camera.Matrix.View * vec4(position, 1);
	projectedPosition /= projectedPosition.w;
	projectedPosition = projectedPosition * 0.5 + 0.5;
	return (projectedPosition.xyz);
}

#define	KERNEL_SIZE 9

uniform vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(0.95581, -0.18159), vec2(0.50147, -0.35807), vec2(0.69607, 0.35559),
	vec2(-0.0036825, -0.59150),	vec2(0.15930, 0.089750), vec2(-0.65031, 0.058189),
	vec2(0.11915, 0.78449),	vec2(-0.34296, 0.51575), vec2(-0.60380, -0.41527));

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

vec4	SSR()
{
	vec3	V = normalize(Frag.Position - Camera.Position);
	vec3	R = reflect(V, Frag.Normal);
	float	RV = dot(R, V);
	float	curLength = 0.15;

	vec4	ret = vec4(0, 0, 1, 0);
	for (uint i = 0; i < MAX_REFLEXION_STEPS; i++)
	{
		vec3	curPos = R * curLength + Frag.Position;
		vec3	curUV = UVFromPosition(curPos);
		//if (curUV.x < 0 || curUV.y < 0 || curUV.x > 1 || curUV.y > 1)
		//	break;
		float	curDepth = sampleLod(LastDepth, curUV.xy, 0.1/* min(0.2, Frag.Material.Roughness * 1.2) */).r;
		for (uint j = 0; j < KERNEL_SIZE; j++)
		{
			uint	h = hash(i + i * j + uint(curDepth + 1000)) % KERNEL_SIZE;
			vec3	poisson3D = normalize(vec3(poissonDisk[h], poissonDisk[h].x * poissonDisk[h].y));
			vec3	sampleR = normalize(R + (poisson3D * 0.0001/* mix(0.0001, 0.0002, Frag.Material.Roughness)*/));
			vec3	samplePos = sampleR * curLength + Frag.Position;
			vec3	sampleUV = UVFromPosition(samplePos);
			if (sampleUV.x < 0 || sampleUV.y < 0 || sampleUV.x > 1 || sampleUV.y > 1)
				continue;
			float	sampleDepth = sampleLod(LastDepth, sampleUV.xy, 0.1/* min(0.2, Frag.Material.Roughness * 1.2) */).r;
			if (sampleDepth == 1)
				continue;
			//if (abs(curUV.z - sampleDepth) <= 0.1)
			if (distance(curPos, samplePos) <= 0.1)
			{
				ret.xyz = sampleUV.xyz;
				ret.w = RV/*  * min(0.5, 1 - Frag.Material.Roughness) */;
				break;
			}
		}
		//curLength += min(Frag.Depth, 0.15);
		//curLength += 0.025;
		curLength = curLength + length(Frag.Position - Position(curUV.xy, curDepth)) * 0.045;
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
	vec3	fresnel = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);
	reflection *= fresnel;
	brightness = dot(pow(reflection_spec, envGammaCorrection), brightnessDotValue);
	reflection_spec *= brightness * min(fresnel + 1, fresnel * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= fresnel * BRDF.x + mix(vec3(1), fresnel, Frag.Material.Metallic) * BRDF.y;
	//specular += reflection_spec;
	diffuse *= Frag.Material.Albedo.rgb * (1 - Frag.Material.Metallic);

	float	alpha = Frag.Material.Alpha + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);

	vec3	envReflection = (specular + reflection_spec + reflection) * alpha;

	vec4	ssrResult = vec4(0, 0, 1, 0);
	if (Frag.Material.Roughness < 1) {
		ssrResult = SSR();
	}
	if (ssrResult.w > 0) {
		vec3	ssrReflection = sampleLod(LastColor, ssrResult.xy, Frag.Material.Roughness * 2).rgb + sampleLod(LastEmitting, ssrResult.xy, Frag.Material.Roughness).rgb;
		//float	screenEdgeFactor = smoothstep(0, 1, 1 - pow(length(ssrResult.xy * 2 - 1), 10));
		float	screenEdgeFactor = 1;
		ssrResult.xy = ssrResult.xy * 2 - 1;
		screenEdgeFactor -= smoothstep(0, 1, pow(abs(ssrResult.x), 10.f));
		screenEdgeFactor -= smoothstep(0, 1, pow(abs(ssrResult.y), 10.f));
		float	reflectionFactor = ssrResult.w * screenEdgeFactor;
		//ssrReflection = mix(ssrReflection * fresnel, envReflection, Frag.Material.Roughness * 0.5);
		ssrReflection *= fresnel;
		Out.Color.rgb += mix(envReflection, ssrReflection, clamp(reflectionFactor, 0, 1));
	}
	else {
		Out.Color.rgb += envReflection;
	}
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
	Out.Color.a = 1;
	//vec4 ssr = SSR();
	//Out.Color.rgb = ssr.xyz * ssr.w;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}
