
struct t_Light {
	vec3	Position;
	vec3	Color;
	int		Type;
#ifdef SHADOW
	int		ShadowIndex;
	mat4	Projection;
#endif //SHADOW
};

#define PointLight			0
#define DirectionnalLight	1

uniform t_Light			Light[LIGHTNBR];
#ifdef SHADOW
uniform sampler2DShadow	Shadow[SHADOWNBR];
#endif //SHADOW
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

#ifdef SHADOW
float	SampleShadowMap(in t_Light light)
{
	if (light.ShadowIndex < 0)
		return (1);
	vec4	shadowPos = light.Projection * vec4(Frag.Position, 1.0);
	vec3	projCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
	return (texture(Shadow[light.ShadowIndex], vec3(projCoord.xy, projCoord.z - 0.001)));
}
#endif //SHADOW

void	ApplyTechnique()
{
	if (Frag.Material.Alpha == 0) {
		return ;
	}
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
	vec3	fresnel = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);
	vec3	diffuse = vec3(0);
	vec3	specular = vec3(0);
	vec3	reflection = vec3(0);

	for (int i = 0; i < LIGHTNBR; i++)
	{
		float	Attenuation = 1;
		#ifdef SHADOW
			Attenuation *= SampleShadowMap(Light[i]);
			if (Attenuation == 0)
				continue ;
		#endif //SHADOW
		bvec3	isZero = equal(Light[i].Color, vec3(0));
		if (isZero.r && isZero.g && isZero.b) {
			continue ;
		}
		vec3	L = Light[i].Position;
		
		if (Light[i].Type == PointLight) {
			L -= Frag.Position;
			Attenuation = length(L);
			Attenuation = 1.0 / (Attenuation * Attenuation);
		}
		if (Attenuation <= 0.01)
			continue ;
		L = normalize(L);
		N = Frag.Normal;
		float	NdL = dot(N, L);
		NdL = max(0, NdL);
		vec3	H = normalize(L + V);
		float	NdH = max(0, dot(N, H));
		float	LdH = max(0, dot(L, H));
		vec3	lightColor = Light[i].Color * Attenuation;
		diffuse += lightColor * NdL * Frag.Material.Albedo * (1 - Frag.Material.Metallic);
		specular += lightColor * min(fresnel + 1, fresnel * Specular(LdH, NdH, Frag.Material.Roughness));
	}

	float	alpha = Frag.Material.Alpha + dot(specular, brightnessDotValue);
	alpha = min(1, alpha);

	Out.Color.rgb += (specular + diffuse + reflection) * alpha;
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Material.Emitting;
	Out.Emitting.a = 1;
}
