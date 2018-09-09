uniform vec3	brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
uniform vec3	envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

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
	float	NdV = max(0, dot(Frag.Normal, V));
	vec3	R = reflect(V, Frag.Normal);

	const vec2	BRDF = BRDF(NdV, Frag.Material.Roughness);

	vec3	diffuse = Frag.Material.AO * (sampleLod(Texture.Environment.Diffuse, -Frag.Normal, Frag.Material.Roughness + 0.9).rgb
			+ texture(Texture.Environment.Irradiance, -Frag.Normal).rgb);
	vec3	reflection = sampleLod(Texture.Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = texture(Texture.Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;


	float	brightness = 0;

	if (Frag.Material.Alpha == 0) {
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

	/* vec3	diffuse = vec3(0);
	vec3	specular = vec3(0);
	vec3	reflection = vec3(0);

	for (int i = 0; i < LIGHTNBR; i++)
	{
		bvec3	isZero = equal(Light[i].Color, vec3(0));
		if (isZero.r && isZero.g && isZero.b)
			continue ;
		vec3	L = (Light[i].Position) + vec3(cos(Time * 1.1), cos(Time * 0.1), cos(Time * 1.7)) - Frag.Position;
		float	Attenuation = 1.0 / length(L);
		L = normalize(L);
		vec3	H = normalize(L + V);
		float	NdH = max(0, dot(Frag.Normal, H));
		float	NdL = max(0, dot(Frag.Normal, L));
		float	LdH = max(0, dot(L, H));
		diffuse += Light[i].Color * NdL * Frag.Material.Albedo * (1 - Frag.Material.Metallic);
		specular += Light[i].Color * min(fresnel + 1, fresnel * Specular(LdH, NdH, Frag.Material.Roughness));
	} */

	Out.Color += specular + diffuse + reflection;
	//Out.Color += mix(EnvDiffuse, Out.Color.rgb, Frag.Material.Alpha);
	Out.Emitting += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
}
