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
	const vec3	EnvDiffuse = texture(Environment.Diffuse, Frag.CubeUV).rgb;

	Frag.Material.AO = 1 - Frag.Material.AO;
	
	vec3	V = normalize(in_CamPos - Frag.Position);
	float	NdV = max(0, dot(Frag.Normal, V));
	vec3	R = reflect(V, Frag.Normal);

	const vec2	BRDF = BRDF(NdV, Frag.Material.Roughness);

	vec3	diffuse = Frag.Material.AO * (sampleLod(Environment.Diffuse, -Frag.Normal, Frag.Material.Roughness + 0.9).rgb
			+ texture(Environment.Irradiance, -Frag.Normal).rgb);
	vec3	reflection = sampleLod(Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = texture(Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;


	float	brightness = 0;

	if (Frag.Material.Albedo.a == 0) {
		Out.Color = EnvDiffuse;
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

	Out.Color = specular + diffuse + reflection;
	Out.Color = mix(EnvDiffuse, Out.Color.rgb, Frag.Material.Albedo.a);
	Out.Emitting = max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}
