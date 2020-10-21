R""(
#if SHADOW_QUALITY == 1
#define SHADOW_SAMPLES	1
#elif SHADOW_QUALITY == 2
#define SHADOW_SAMPLES	3
#elif SHADOW_QUALITY == 3
#define SHADOW_SAMPLES	5
#else //SHADOW_QUALITY == 4
#define SHADOW_SAMPLES	9
#endif

#define	KERNEL_SIZE		9

const vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(0.95581, -0.18159), vec2(0.50147, -0.35807), vec2(0.69607, 0.35559),
	vec2(-0.0036825, -0.59150),	vec2(0.15930, 0.089750), vec2(-0.65031, 0.058189),
	vec2(0.11915, 0.78449),	vec2(-0.34296, 0.51575), vec2(-0.60380, -0.41527));

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
const vec3	brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
const vec3	envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

float	GGX_Geometry(in float NdV, in float alpha2)
{
	return NdV / (NdV * (1 - alpha2) + alpha2);
}

float	GGX_Distribution(in float NdH, in float alpha2)
{
	float den = (NdH * NdH) * (alpha2 - 1) + 1;
	return (alpha2 / (M_PI * den * den));
}

vec3 BRDFFresnel(const in vec3 F0, const in float factor)
{
	return F0 + (1 - F0) * pow(1 - factor, 5);
}

float BRDFSpecular(const in float NdH, const in float NdV)
{
	float	alpha2 = Alpha() * Alpha();
	float	D = GGX_Distribution(NdH, alpha2);
	float	G = GGX_Geometry(NdV, alpha2);
	return (D * G);
}

#ifdef SHADOW
float	SampleShadowMap(in int lightIndex)
{
	if (Light[lightIndex].ShadowIndex < 0)
		return (1);
	vec4	shadowPos = Light[lightIndex].Projection * vec4(WorldPosition(), 1.0);
	vec3	projCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
	return (texture(Shadow[Light[lightIndex].ShadowIndex], vec3(projCoord.xy, projCoord.z - 0.001)));
}

float	SampleShadowMap(in int lightIndex, in vec2 sampleRotation)
{
	if (Light[lightIndex].ShadowIndex < 0)
		return (1);
	vec4	shadowPos = Light[lightIndex].Projection * vec4(WorldPosition(), 1.0);
	vec3	projCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
	float	sampleOffset = 1 / 512.f;
	float	shadow = 0;
	for (int i = 0; i < SHADOW_SAMPLES; i++)
	{
		vec2	sampleUV = projCoord.xy + poissonDisk[i] * sampleRotation * sampleOffset;
		vec4	sampleDepth = textureGather(Shadow[Light[lightIndex].ShadowIndex], sampleUV, projCoord.z - 0.001);
		//shadow += (sampleDepth[0] + sampleDepth[1] + sampleDepth[2] + sampleDepth[2]) / 4.f;
		shadow += texture(Shadow[Light[lightIndex].ShadowIndex], vec3(sampleUV, projCoord.z - 0.001));
	}
	return (shadow / float(SHADOW_SAMPLES));
}
#endif //SHADOW

void	Lighting()
{
	if (Opacity() == 0) {
		return ;
	}
	vec3	V = normalize(Camera.Position - WorldPosition());
	vec3	N = WorldNormal();
	float	NdV = dot(N, V);
	if (Opacity() < 1 && NdV < 0)
	{
		N = -N;
		NdV = -NdV;
	}
	NdV = max(0, NdV);
	vec3	R = reflect(V, N);
	vec3	diffuse = vec3(0);
	vec3	specular = vec3(0);
	vec3	reflection = vec3(0);

	float	sampleAngle = randomAngle(WorldPosition());
	float	s = sin(sampleAngle);
	float	c = cos(sampleAngle);
	vec2	sampleRotation = vec2(c, -s);
	for (int i = 0; i < LIGHTNBR; i++)
	{
		float	Attenuation = 1;
		#ifdef SHADOW
			Attenuation *= SampleShadowMap(i, sampleRotation);
			if (Attenuation == 0)
				continue ;
		#endif //SHADOW
		bvec3	isZero = equal(Light[i].Color, vec3(0));
		if (all(isZero)) {
			continue ;
		}
		vec3	L = Light[i].Position;
		
		if (Light[i].Type == PointLight) {
			L -= WorldPosition();
			Attenuation = length(L);
			Attenuation = 1.0 / (Attenuation * Attenuation);
		}
		if (Attenuation <= 0.01)
			continue ;
		L = normalize(L);
		N = WorldNormal();
		vec3	H = normalize(L + V);
		float	NdL = max(0, dot(N, L));
		float	VdH = max(0, dot(V, H));
		float	NdH = max(0, dot(N, H));
		float	LdH = max(0, dot(L, H));
		vec3	lightColor = Light[i].Color * Attenuation;
		vec3	F = BRDFFresnel(F0(), LdH);
		specular += lightColor * (F * BRDFSpecular(NdH, NdV));
		//diffuse += lightColor * ((1 - F) * CDiff() / M_PI);
		diffuse += lightColor * NdL * CDiff();
	}

	float	alpha = Opacity() + dot(specular, brightnessDotValue);
	alpha = min(1, alpha);

	vec4 backColor = BackColor();
	vec3 backEmitting = BackEmitting().rgb;

	
	backColor.rgb += (specular + diffuse + reflection) * alpha;
	backColor.a = 1;
	backEmitting.rgb += max(vec3(0), BackColor().rgb - 1);
	SetBackColor(backColor);
	SetBackEmitting(backEmitting);
}

)""