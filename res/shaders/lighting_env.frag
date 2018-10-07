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
		depth = sampleLod(LastDepth, projectedCoord.xy, 0).r;
		if (depth == 1)
			continue;
		dDepth = projectedCoord.z - depth;
		dir *= 0.5;
		if(dDepth >= 0.0) {
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

vec4	SSR()
{
	float	dDepth;
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	reflectDir = -(mat3(Camera.Matrix.View) * normalize(reflect(V, Frag.Normal)));
	vec3	hitPos = (Camera.Matrix.View * vec4(Frag.Position, 1)).xyz;
	vec4	projectedCoord;
	vec4	ret = vec4(0, 0, 1, -reflectDir.z);

	//vec3	maxStep = hitPos + (reflectDir * ac * 30.f);
	//vec3	maxStep = hitPos + (reflectDir * 30.f) * Frag.Depth;
	/* vec2	nReflectDir = normalize(reflectDir.xy);
	float	aA = atan(nReflectDir.x, nReflectDir.y) * 180.f / M_PI;
	float	cA = 180 - (90 + aA);
	vec2	a = Frag.UV;
	vec2	b = sign(a);
	float	ac = length(a - b) / sin(cA * M_PI / 180.f);
	reflectDir *= Frag.Depth; */
	reflectDir *= max(0.1, Frag.Depth * 0.25);
	for (int i = 0; i < maxSteps; i++)
	{
		hitPos += reflectDir;
		projectedCoord = Camera.Matrix.Projection * vec4(hitPos, 1.0);
		projectedCoord /= projectedCoord.w;
		projectedCoord = projectedCoord * 0.5 + 0.5;
		if (projectedCoord.x > 1 || projectedCoord.x < 0 || projectedCoord.y > 1 || projectedCoord.y < 0)
			break;
		float sampleDepth = sampleLod(LastDepth, projectedCoord.xy, 0).r;
		if (sampleDepth == 1)
			continue;
		dDepth = projectedCoord.z - sampleDepth;
		if (dDepth >= 0) {
			return vec4(BinarySearch(reflectDir, hitPos, dDepth), ret.w * smoothstep(0, 1, 1 - i / float(maxSteps)));
			break;
		}
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
	if (ssrResult.z != 1) {
		vec3	ssrReflection = sampleLod(LastColor, ssrResult.xy, Frag.Material.Roughness * 2).rgb + sampleLod(LastEmitting, ssrResult.xy, Frag.Material.Roughness * 1.5).rgb;
		//vec3	ssrReflection = sampleLod(LastColor, ssrResult.xy, Frag.Material.Roughness * 2).rgb + texture(LastEmitting, ssrResult.xy).rgb;
		//float	screenEdgeFactor = smoothstep(0, 1, 1 - pow(length(ssrResult.xy * 2 - 1), 10));
		/* float	screenEdgeFactor = 1;
		vec2	dCoord = abs(ssrResult.xy * 2 - 1);
		if (dCoord.x > 1 || dCoord.x < 0)
			screenEdgeFactor -= mod(dCoord.x, 1) * 2;
		if (dCoord.y > 1 || dCoord.y < 0)
			screenEdgeFactor -= mod(dCoord.y, 1) * 2; */
		float	screenEdgeFactor = smoothstep(0, 1, 1 - pow(length(ssrResult.xy * 2 - 1), 10));
		float	reflectionFactor = ssrResult.w * screenEdgeFactor;
		Out.Color.rgb += mix(envReflection, ssrReflection * fresnel, clamp(reflectionFactor, 0, 1));
		//Out.Color.rgb = ssrReflection;
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