R""(
const vec3		envGammaCorrection = vec3(2.2);
uniform sampler2D	SSRResult;

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

vec3	Fresnel(in float cosT, in vec3 F0)
{
  return (F0 + (1 - F0) * pow(1 - cosT, 5));
}

vec3	F0(in float ior)
{
	float	f0 = abs((1.0 - ior) / (1.0 + ior));
	return (vec3(f0 * f0));
}

vec4	SSR()
{
	return texture(SSRResult, Frag.UV, 0);
}

float	warpUV(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (smoothstep(min, max, 1 - mod(percent, 1)));
	return (percent);
}

vec2	warpUV(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(warpUV(min.x, max.x, percent.x), warpUV(min.y, max.y, percent.y)));
}

vec2	GetRefractUV()
{
	vec2	refract_UV = Frag.UV;

	if (Frag.Material.Ior > 1)
	{
		vec3	V = normalize(Camera.Position - Frag.Position);
		float	NdV = dot(Frag.Normal, V);
		if (NdV < 0) {
			Frag.Normal = -Frag.Normal;
			NdV = -NdV;
		}
		vec3	fresnel = Fresnel(NdV, F0(Frag.Material.Ior));
		vec2	refractFactor = vec2((1 - Frag.Depth) * max(fresnel.x, max(fresnel.y, fresnel.z)));
		vec2	refractDir = (mat3(Camera.Matrix.View) * normalize(refract(V, Frag.Normal, 1.0 / Frag.Material.Ior))).xy;
		refract_UV = refractDir * refractFactor + Frag.UV;
		refract_UV = warpUV(vec2(0), vec2(1), refract_UV);
	}
	return refract_UV;
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
	vec4	ssrResult = SSR();
	vec3	diffuse = Frag.Material.AO * texture(Texture.Environment.Irradiance, -N).rgb;
	vec3	reflection = sampleLod(Texture.Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = sampleLod(Texture.Environment.Irradiance, R, Frag.Material.Roughness).rgb;
	vec2	UV = GetRefractUV();
	vec3	fresnel = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);
	reflection *= fresnel;
	float brightness = Luminance(pow(specular, envGammaCorrection));
	specular *= brightness * min(fresnel, fresnel * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= fresnel * brdf.x + brdf.y;
	diffuse *= DiffuseFactor();

	float	alpha = Frag.Material.Alpha + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);

	vec3	envReflection = (specular /* + reflection_spec */ + reflection) * alpha;
	
	Out.Color.rgb += mix(envReflection, ssrResult.xyz * fresnel, ssrResult.w);
	Out.Color.rgb += (diffuse + Frag.Material.Emitting) * alpha;
#ifndef TRANSPARENT
	Out.Color.rgb += EnvDiffuse * floor(Frag.Depth) * (1 - alpha);
#endif
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}

)""
