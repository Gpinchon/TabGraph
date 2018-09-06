float	cmix(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (mix(min, max, 1 - mod(percent, 1)));
	return (mix(min, max, percent));
}

vec2	cmix(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(cmix(min.x, max.x, percent.x), cmix(min.y, max.y, percent.y)));
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

const vec3		brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
const vec3		envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

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
	Frag.Material.AO = 1 - Frag.Material.AO;

	vec3	V = normalize(in_CamPos - Frag.Position);
	float	NdV = dot(Frag.Normal, V);
	if (NdV < 0) {
		Frag.Normal = -Frag.Normal;
		NdV = -NdV;
	}

	const vec2	BRDF = BRDF(NdV, Frag.Material.Roughness);

	vec3	diffuse = Frag.Material.AO * (sampleLod(Environment.Diffuse, -Frag.Normal, Frag.Material.Roughness + 0.9).rgb
			+ texture(Environment.Irradiance, -Frag.Normal).rgb);
	diffuse *= Frag.Material.Albedo.a;
	vec3	R = reflect(V, Frag.Normal);
	vec3	reflection = sampleLod(Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = texture(Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;

	vec3	fresnel = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);

	vec2	refract_UV = Frag.UV;

	if (Frag.Material.Ior > 1)
	{
		vec2	refractFactor = vec2(1 - Frag.Depth) * vec2(0.25f) + (fresnel.x + fresnel.y + fresnel.z) / 3.f * 0.0125f;
		vec2	refractDir = (mat3(in_ViewMatrix) * normalize(refract(V, Frag.Normal, 1.0 / Frag.Material.Ior))).xy;
		refract_UV = refractDir * refractFactor + Frag.UV;
		refract_UV = warpUV(vec2(0), vec2(1), refract_UV);
	}

	vec3	Back_Color = sampleLod(in_Back_Color, refract_UV, Frag.Material.Roughness).rgb;
	vec3	Back_Bright = sampleLod(in_Back_Bright, refract_UV, Frag.Material.Roughness).rgb;

	if (Frag.Material.Albedo.a == 0) {
		Out.Color.rgb = Back_Color;
		Out.Emitting.rgb = Back_Bright;
		return ;
	}

	float	brightness = 0;

	reflection *= fresnel;
	brightness = dot(pow(reflection_spec, envGammaCorrection), brightnessDotValue);
	reflection_spec *= brightness * min(fresnel + 1, fresnel * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= fresnel * BRDF.x + mix(vec3(1), fresnel, Frag.Material.Metallic) * BRDF.y;
	specular += reflection_spec;
	diffuse *= Frag.Material.Albedo.rgb * (1 - Frag.Material.Metallic);
	Frag.Material.Albedo.a += dot(specular, specular);
	Frag.Material.Albedo.a = min(1, Frag.Material.Albedo.a);

	float	mappedAlpha = map(Frag.Material.Albedo.a, 0, 1, 0.5, 1);
	Back_Color = mix(Back_Color, Back_Color * Frag.Material.Albedo.rgb, mappedAlpha);
	Back_Bright = mix(Back_Bright, Back_Bright * Frag.Material.Albedo.rgb, mappedAlpha);

	Out.Color.rgb = specular + diffuse + reflection + Frag.Material.Emitting;
	Out.Color.rgb = mix(Back_Color, Out.Color.rgb, Frag.Material.Albedo.a);
	Out.Emitting.rgb = max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
	Out.Emitting.rgb = mix(Back_Bright, Out.Emitting.rgb, Frag.Material.Albedo.a);
}