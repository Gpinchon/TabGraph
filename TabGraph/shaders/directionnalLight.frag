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
	vec3	Direction;
	vec3	Color;
	vec3	Min;
	vec3	Max;
	bool	Infinite;
#ifdef SHADOW
	mat4	Projection;
	sampler2DShadow	Shadow;
#endif //SHADOW
};

uniform t_Light			Light;

#ifdef SHADOW
float	SampleShadowMap()
{
	vec4	shadowPos = Light.Projection * vec4(WorldPosition(), 1.0);
	vec3	projCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
	return (texture(Light.Shadow, vec3(projCoord.xy, projCoord.z - 0.001)));
}

float	SampleShadowMap(in vec2 sampleRotation, float bias)
{
	vec4	shadowPos = Light.Projection * vec4(WorldPosition(), 1.0);
	vec3	projCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
	float	sampleOffset = 1 / 512.f;
	float	shadow = 0;
	for (int i = 0; i < SHADOW_SAMPLES; i++)
	{
		vec2	sampleUV = projCoord.xy + poissonDisk[i] * sampleRotation * sampleOffset;
		//vec4	sampleDepth = textureGather(Light.Shadow, sampleUV, projCoord.z - 0.001);
		//shadow += (sampleDepth[0] + sampleDepth[1] + sampleDepth[2] + sampleDepth[2]) / 4.f;
		shadow += texture(Light.Shadow, vec3(sampleUV, projCoord.z - bias));
	}
	return (shadow / float(SHADOW_SAMPLES));
}
#endif //SHADOW

float SpecularFactor(const in float NdotH, const in float SpecularPower)
{
	return ((SpecularPower + 2) / 8 ) * pow(clamp(NdotH, 0, 1), SpecularPower);
}

float GlossToSpecularPower(const in float gloss)
{
	return exp2(10 * gloss + 1);
}

void	Lighting()
{
	vec3	L = Light.Direction;
	vec3	V = normalize(Camera.Position - WorldPosition());
	vec3	N = WorldNormal();	
	float	Attenuation = 1;
	float	NdotL = dot(N , L);
	if (Opacity() < 1 && NdotL < 0) {
		NdotL = -NdotL;
		N = -N;
	}
	NdotL = max(NdotL, 0);
	#ifdef SHADOW
		float	sampleAngle = randomAngle(WorldPosition());
		float	s = sin(sampleAngle);
		float	c = cos(sampleAngle);
		float bias = 0.01 * tan(acos(NdotL));
		Attenuation *= SampleShadowMap(vec2(c, -s), clamp(bias, 0.0f, 0.02f));
		if (Attenuation == 0)
			return ;
	#endif //SHADOW
	bool isAboveMax = any(greaterThan(WorldPosition(), Light.Max));
	bool isUnderMin = any(lessThan(WorldPosition(), Light.Min));
	if (!Light.Infinite && (isAboveMax || isUnderMin)) {
		return ;
	}
	bvec3	isZero = equal(Light.Color, vec3(0));
	if (all(isZero)) {
		return ;
	}
	vec3	H = normalize(V + L);
	float	NdotH = max(dot(N , H), 0.0);
	vec3	diffuse = Light.Color * Attenuation * NdotL;
	float	SpecularPower = GlossToSpecularPower(1 - sqrt(Alpha()));

	out_0 = vec4(diffuse, SpecularFactor(NdotH, SpecularPower) * Attenuation);
	out_1 = vec4(0);
}

)""