uniform vec3	brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
uniform vec3	envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

uniform sampler2D	LastColor;
uniform sampler2D	LastEmitting;
uniform sampler2D	LastDepth;

uniform float	step = 0.1;
uniform float	minRayStep = 0.1;
uniform int		maxSteps = 30;
uniform float	searchDist = 5;
uniform int		numBinarySearchSteps = 5;
uniform float	reflectionSpecularFalloffExponent = 3.0;

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
	float depth;

	vec4 projectedCoord;
	for(int i = 0; i < numBinarySearchSteps; i++)
	{
		projectedCoord = Camera.Matrix.Projection * vec4(hitCoord, 1.0);
		projectedCoord /= projectedCoord.w;
		projectedCoord = projectedCoord * 0.5 + 0.5;
		if (projectedCoord.x > 1 || projectedCoord.x < 0 || projectedCoord.y > 1 || projectedCoord.y < 0)
			continue;
		depth = sampleLod(Texture.Depth, projectedCoord.xy, 0.1).r;
		if (depth == 1)
			continue;
		dDepth = projectedCoord.z - depth;
		dir *= 0.5;
		if(dDepth > 0.0) {
			hitCoord += dir;
		}
		else {
			hitCoord -= dir;
		}
	}

	projectedCoord = Camera.Matrix.Projection * vec4(hitCoord, 1.0);
	projectedCoord.xy /= projectedCoord.w;
	projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

	return vec3(projectedCoord.xy, depth);
}

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

vec4	SSR()
{
	vec3	V = normalize(Frag.Position - Camera.Position);
	vec3	reflectDir = reflect(V, Frag.Normal);
	float	curLength = 0.25;

	vec4	ret = vec4(0, 0, 1, 0);
	for (int i = 0; i < maxSteps; i++)
	{
		vec3	curPos = Frag.Position + reflectDir * curLength;
		vec3	curUV = UVFromPosition(curPos);
		float	curDepth = texture(LastDepth, curUV.xy).r;
		if (curDepth == 1)
			continue;
		vec3	newPos = Position(curUV.xy);
		vec3	tempPos = newPos;
		for (int i = 0; i < KERNEL_SIZE; i++)
		{
			//if (abs(curUV.z - curDepth) <= 0.001)
			if (distance(tempPos, newPos) <= 0.001)
			{
				ret.xyz = curUV.xyz;
				ret.w = dot(reflectDir, V) /* * (1 - Frag.Material.Roughness) */;
				break;
			}
			curUV.xy = curUV.xy + (poissonDisk[i] * 0.1);
			curDepth = texture(LastDepth, curUV.xy).r;
			tempPos = Position(curUV.xy, curDepth);
			
		}
		/* if (distance(curPos, newPos) <= 0.001)
		{
			return (vec4(curUV, 1));
		} */
		/* vec3	newPos = Position(curUV.xy);
		if (distance(curPos, newPos) <= 0.001)
		{
			return (vec4(curUV, 1));
		}*/
		/* float	curDepth = texture(Texture.Depth, curUV.xy).r;
		if (abs(curUV.z - curDepth) <= 0.001)
		{
			return (vec4(curUV, 1));
		}
		vec3	newPos = Position(curUV.xy, curDepth); */
		curLength = length(Frag.Position - newPos);
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
		float	screenEdgeFactor = smoothstep(0, 1, 1 - pow(length(ssrResult.xy * 2 - 1), 10));
		float	reflectionFactor = ssrResult.w * screenEdgeFactor;
		ssrReflection = mix(ssrReflection * fresnel, envReflection, Frag.Material.Roughness * 0.5);
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
