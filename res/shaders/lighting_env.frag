uniform vec3	brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
uniform vec3	envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

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
		depth = texture(Texture.Depth, projectedCoord.xy).r;
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

vec3	SSR()
{
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	reflectDir = -normalize(mat3(Camera.Matrix.View) * normalize(reflect(V, Frag.Normal)));
	vec4	hitPos = Camera.Matrix.View * vec4(Frag.Position, 1);
	float	dDepth;
	vec4	projectedCoord;

	reflectDir *= step;
	for (int i = 0; i < maxSteps && reflectDir.z < 0; i++)
	{
		hitPos.xyz += reflectDir;
		projectedCoord = Camera.Matrix.Projection * hitPos;
		projectedCoord /= projectedCoord.w;
		projectedCoord = projectedCoord * 0.5 + 0.5;
		float sampleDepth = texture(Texture.Depth, projectedCoord.xy).r;
		if (sampleDepth == 1)
			continue;
		dDepth = projectedCoord.z - sampleDepth;
		if (dDepth > 0) {
			return (BinarySearch(reflectDir, hitPos.xyz, dDepth));
		}
	}
	return (vec3(0, 0, 1));
	//if (result.z != 1)
	//	Out.Color.rgb += sampleLod(Texture.Back.Color, result.xy, Frag.Material.Roughness).rgb;
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
	specular += reflection_spec;
	diffuse *= Frag.Material.Albedo.rgb * (1 - Frag.Material.Metallic);

	float	alpha = Frag.Material.Alpha + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);

	vec3	envReflection = (specular + reflection) * alpha;

	vec3	ssrResult = vec3(0, 0, 1);
	if (Frag.Material.Roughness < 1) {
		ssrResult = SSR();
	}
	if (ssrResult.z != 1) {
		vec3	ssrReflection = sampleLod(Texture.Back.Color, ssrResult.xy, Frag.Material.Roughness).rgb + sampleLod(Texture.Back.Emitting, ssrResult.xy, Frag.Material.Roughness).rgb;
		Out.Color.rgb += 
			mix(ssrReflection * fresnel, envReflection, Frag.Material.Roughness) * Frag.Material.Alpha;
	}
	else {
		Out.Color.rgb += envReflection;
	}
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
	
}