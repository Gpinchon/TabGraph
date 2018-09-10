
struct t_Light {
	vec3	Position;
	vec3	Color;
};

#define LIGHTNBR 32

uniform t_Light	Light[LIGHTNBR];
/* uniform vec3	LightPositions[LIGHTNBR];
uniform vec3	LightColors[LIGHTNBR]; */
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

float	GGX_Geometry(in float NdV, in float alpha)
{
	float	alpha2 = alpha * alpha;
	return (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
}

float	GGX_Distribution(in float NdH, in float alpha)
{
	float den = (NdH * NdH) * (alpha - 1) + 1;
	return (alpha / (M_PI * den * den));
}

float	Specular(in float NdV, in float NdH, in float roughness)
{
	float	alpha = roughness * roughness;
	float	D = GGX_Distribution(NdH, alpha);
	float	G = GGX_Geometry(NdV, alpha);
	return (max(D * G, 0));
}

void	ApplyTechnique()
{
	if (Frag.Material.Alpha == 0) {
		discard;
	}
	const vec3	EnvDiffuse = texture(Texture.Environment.Diffuse, Frag.CubeUV).rgb;

	Frag.Material.AO = 1 - Frag.Material.AO;
	
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	N = Frag.Normal;
	float	NdV = dot(N, V);
	if (Frag.Material.Alpha < 1 && NdV < 0)
	{
		N = -N;
		NdV = -NdV;
	}
	NdV = max(0, NdV);
	vec3	R = reflect(V, N);

	const vec2	BRDF = BRDF(NdV, Frag.Material.Roughness);

	vec3	fresnel = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);
	vec3	diffuse = vec3(0);
	vec3	specular = vec3(0);
	vec3	reflection = vec3(0);

	for (int i = 0; i < LIGHTNBR; i++)
	{
		bvec3	isZero = equal(Light[i].Color, vec3(0));
		if (isZero.r && isZero.g && isZero.b) {
			continue ;
		}
		vec3	L = (Light[i].Position) /* + vec3(cos(Time * 1.1), cos(Time * 0.1), cos(Time * 1.7)) */ - Frag.Position;
		float	Attenuation = 1.0 / length(L);
		L = normalize(L);
		N = Frag.Normal;
		float	NdL = dot(N, L);
		/* if (Frag.Material.Alpha < 1 && NdL < 0)
		{
			N = -N;
			NdL = -NdL;
		} */
		NdL = max(0, NdL);
		vec3	H = normalize(L + V);
		float	NdH = max(0, dot(N, H));
		float	LdH = max(0, dot(L, H));
		diffuse += Light[i].Color * NdL * Frag.Material.Albedo * (1 - Frag.Material.Metallic);
		specular += Light[i].Color * min(fresnel + 1, fresnel * Specular(LdH, NdH, Frag.Material.Roughness));
	}

	float	alpha = Frag.Material.Alpha + max(specular.r, max(specular.g, specular.b));//length(specular);
	alpha = min(1, alpha);

	Out.Color.rgb += (specular + diffuse + reflection) * alpha;
	Out.Color.a = 1;
	//Out.Color.a = max(Out.Color.a, alpha);
	//Out.Color = mix(EnvDiffuse, Out.Color.rgb, alpha);
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
	Out.Emitting.a = 1;
	//Out.Emitting.a = Out.Color.a;
}
