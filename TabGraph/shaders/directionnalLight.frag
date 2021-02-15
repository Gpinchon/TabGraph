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

vec2 Hammersley(uint Index, uint NumSamples, uvec2 Random)
{
	float E1 = fract( Index / float(NumSamples) + float( Random.x & 0xffff ) / (1<<16) );
	float E2 = float( bitfieldReverse(Index) ^ Random.y ) * 2.3283064365386963e-10;
	return vec2( E1, E2 );
}

uvec3 Rand3DPCG16(ivec3 p)
{
	uvec3 v = uvec3(p);
	v = v * 1664525u + 1013904223u;
	v.x += v.y*v.z;
	v.y += v.z*v.x;
	v.z += v.x*v.y;
	v.x += v.y*v.z;
	v.y += v.z*v.x;
	v.z += v.x*v.y;
	return v >> 16u;
}

float	SampleShadowMap(float bias)
{
	vec4	shadowPos = Light.Projection * vec4(WorldPosition(), 1.0);
	vec3	projCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
	float	sampleOffset = 5.f / 256.f;
	float	shadow = 0;
	uvec2	Random = Rand3DPCG16(ivec3(gl_FragCoord.xy, FrameNumber % 8)).xy;
	for (int i = 0; i < SHADOW_SAMPLES; i++)
	{
		vec2	sampleUV = projCoord.xy + Hammersley(i, SHADOW_SAMPLES, Random) * sampleOffset;
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
		float bias = 0.01 * tan(acos(NdotL));
		Attenuation *= SampleShadowMap(clamp(bias, 0.0f, 0.02f));
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