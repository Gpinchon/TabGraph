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

const vec3	brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
const vec3	envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

const float DIV_PI = 1.0 / M_PI;
vec3 BRDFfresnelSchlick(const in vec3 f0, const in  float f90, const in  float u)
{
    return f0 + (f90 - f0) * pow(clamp(1.f - u, 0.f, 1.f), 5.f);
}
// Diffuse BRDF
// ----------------------------------------------------------------------------
float BRDFDiffuse(float NdotV, float NdotL, float LdotH, float linearRoughness)
{    
    float energyBias = mix(0.f, 0.5, linearRoughness);
    float energyFactor = mix(1.0, 1.0/1.51, linearRoughness);
    float fd90 = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
    vec3 f0 = vec3 (1.0, 1.0, 1.0);
    float lightScatter = BRDFfresnelSchlick(f0, fd90, NdotL).r;
    float viewScatter = BRDFfresnelSchlick(f0, fd90, NdotV).r;
    return lightScatter * viewScatter * energyFactor;
}
// Specular BRDF Geometry Component
// ----------------------------------------------------------------------------
float BRDFGeometry(float NdotL , float NdotV , float alpha2)
{
	float a = 2 * NdotV;
	float b = NdotV + sqrt(alpha2 + (1 - alpha2) * pow(NdotV, 2));
	return a / (b + 0.0001);
    /*float Lambda_GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - alpha2) + alpha2);
    float Lambda_GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - alpha2) + alpha2);
	float GGX = Lambda_GGXV + Lambda_GGXL;
	if (GGX > 0)
		return 0.5 / GGX;
	return 0;*/
}
// Specular BRDF Distribution Component
// ----------------------------------------------------------------------------
float BRDFDistribution(float NdotH , float alpha2)
{
    // remapping to Quadratic curve
    float f = (NdotH * NdotH) * (alpha2 - 1.0) + 1.0;
    return alpha2 / (M_PI * f * f);
}
// ----------------------------------------------------------------------------
vec3 BRDFLight( vec3 L, vec3 V, vec3 N)
{
    vec3 H = normalize(V + L);
    float NdotV = max(dot(N , V), 0.0);
    float LdotH = max(dot(L , H), 0.0);
    float NdotH = max(dot(N , H), 0.0);
    float NdotL = max(dot(N , L), 0.0);

    // Specular BRDF
    float f90 = 1.0;
    vec3 F = BRDFfresnelSchlick(F0(), f90, LdotH);
    float G = BRDFGeometry(NdotV, NdotL, Alpha());
    float D = BRDFDistribution(NdotH, Alpha());
    vec3 Fr = F * G * D;

    // Diffuse BRDF
    float Fd = BRDFDiffuse(NdotV, NdotL, LdotH , Alpha()); 
    
    return (Fd + Fr) * NdotL * DIV_PI;
}

float BRDFSpecular(const in float NdotV, const in float NdotL, const in float NdotH, const in float LdotH)
{
	float f90 = 1.0;
	float alpha2 = Alpha() * Alpha();
    //vec3 F = BRDFfresnelSchlick(F0(), f90, LdotH);
    float G = BRDFGeometry(NdotV, NdotL, alpha2);
    float D = BRDFDistribution(NdotH, alpha2);
    return /*F **/ G * D;
}

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

void	Lighting()
{
	vec3	L = Light.Direction;
	vec3	V = normalize(Camera.Position - WorldPosition());
	vec3	N = WorldNormal();
	float	NdotV = dot(N, V);
	if (Opacity() < 0.99 && NdotV < 0)
	{
		N = -N;
		NdotV = -NdotV;
	}
	NdotV = max(0, NdotV);
	float	NdotL = max(dot(N , L), 0.0);
	float	sampleAngle = randomAngle(WorldPosition());
	float	s = sin(sampleAngle);
	float	c = cos(sampleAngle);
	vec2	sampleRotation = vec2(c, -s);
	float	Attenuation = 1;
	#ifdef SHADOW
		float bias = 0.01 * tan(acos(NdotL));
		Attenuation *= SampleShadowMap(sampleRotation, clamp(bias, 0.0f, 0.02f));
		if (Attenuation == 0)
			return ;
	#endif //SHADOW
	bool isAboveMax = any(greaterThan(WorldPosition(), Light.Max));
	bool isUnderMin = any(lessThan(WorldPosition(), Light.Min));
	if (!Light.Infinite && (isAboveMax || isUnderMin) || Opacity() == 0) {
		return ;
	}
	bvec3	isZero = equal(Light.Color, vec3(0));
	if (all(isZero)) {
		return ;
	}
	vec3	H = normalize(V + L);
	float	LdotH = max(dot(L , H), 0.0);
	float	NdotH = max(dot(N , H), 0.0);
	vec3	diffuse = Light.Color * Attenuation * NdotL;//BRDFDiffuse(NdotV, NdotL, LdotH, Alpha());
	float	specular = Attenuation * BRDFSpecular(NdotV, NdotL, NdotH, LdotH);
	SetBackColor(vec4(diffuse, specular));
	SetBackEmissive(vec4(0));
}

)""