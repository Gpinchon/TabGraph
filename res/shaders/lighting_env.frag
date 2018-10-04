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
			break;
		depth = sampleLod(LastDepth, projectedCoord.xy, Frag.Material.Roughness).r;
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

vec4	SSR()
{
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	reflectDir = -normalize(mat3(Camera.Matrix.View) * normalize(reflect(V, Frag.Normal)));
	vec4	hitPos = Camera.Matrix.View * vec4(Frag.Position, 1);
	float	dDepth;
	vec4	projectedCoord;
	vec4	ret = vec4(0, 0, 1, -reflectDir.z);

	reflectDir *= /* step *  */Frag.Depth;
	for (int i = 0; i < maxSteps /* && reflectDir.z < 0 */; i++)
	{
		hitPos.xyz += reflectDir;
		projectedCoord = Camera.Matrix.Projection * hitPos;
		projectedCoord /= projectedCoord.w;
		projectedCoord = projectedCoord * 0.5 + 0.5;
		if (projectedCoord.x > 1 || projectedCoord.x < 0 || projectedCoord.y > 1 || projectedCoord.y < 0)
			break;
		float sampleDepth = sampleLod(LastDepth, projectedCoord.xy, Frag.Material.Roughness).r;
		if (sampleDepth == 1)
			continue;
		dDepth = projectedCoord.z - sampleDepth;
		if (dDepth > 0) {
			ret.xyz = BinarySearch(reflectDir, hitPos.xyz, dDepth);
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
		vec3	ssrReflection = sampleLod(LastColor, ssrResult.xy, Frag.Material.Roughness).rgb + sampleLod(LastEmitting, ssrResult.xy, Frag.Material.Roughness).rgb;
		vec2	dCoord = abs(vec2(0.5, 0.5) - ssrResult.xy);
		float	screenEdgeFactor = smoothstep(0, 1, 1 - pow(dCoord.x + dCoord.y, 3));
		float	reflectionFactor = ssrResult.w * length(fresnel) * screenEdgeFactor;
		Out.Color.rgb += mix(envReflection, specular + ssrReflection * fresnel, clamp(reflectionFactor, 0, 1));
	}
	else {
		Out.Color.rgb += envReflection;
	}
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}