#version 450 core
#define M_PI 3.1415926535897932384626433832795

uniform vec3		in_CamPos;
uniform vec3		in_Albedo;
uniform vec2		in_UVScale;
uniform float		in_Roughness;
uniform float		in_Metallic;
uniform float		in_Refraction;
uniform float		in_Alpha;
uniform float		in_Parallax;

uniform bool		in_Use_Texture_Albedo;
uniform bool		in_Use_Texture_Roughness;
uniform bool		in_Use_Texture_Metallic;
uniform bool		in_Use_Texture_Normal;
uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Roughness;
uniform sampler2D	in_Texture_Metallic;
uniform sampler2D	in_Texture_Normal;

uniform samplerCube	in_Texture_Env;
uniform samplerCube	in_Texture_Env_Spec;

in vec3			frag_Normal;
in vec3			frag_Position;
in vec2			frag_Texcoord;
in vec3			frag_Tangent;
in vec3			frag_Bitangent;
in mat3			frag_NormalMatrix;

out vec4		out_Color;

/*float D_GGX(in float NdH, in float roughness)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (M_PI * d * d);
}

float G_schlick(in float NdV, in float NdL, in float roughness)
{
    float k = roughness * roughness * 0.5;
    float V = NdV * (1.0 - k) + k;
    float L = NdL * (1.0 - k) + k;
    return 0.25 / (V * L);
}

float D_blinn(in float NdH, in float roughness)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;
    return (n + 2.0) / (2.0 * M_PI) * pow(NdH, n);
}

vec3 S_Cooktorrance(in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness)
{
    float D = D_GGX(NdH, roughness);
    float G = G_schlick(NdV, NdL, roughness);
    float rim = mix(1.0 - roughness * 0.9, 1.0, NdV);
    return (1.0 / rim) * specular * G * D;
}*/

float chiGGX(float v)
{
    return v > 0 ? 1 : 0;
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

float ndfGGX(float NdH, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (NdH * NdH) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (M_PI * denom * denom);
}

float	GGX_Geometry(in float NdV, in float roughness)
{
	float NdV2 = 2 * NdV;
	float r2 = roughness * roughness;
	float denom = NdV + sqrt(r2 + (1 - r2) * (NdV * NdV));
	return (NdV / denom);
	/*float VoH2 = clamp(HdV, 0, 1);
	float chi = chiGGX( VoH2 / clamp(NdV, 0, 1) );
	VoH2 = VoH2 * VoH2;
	float tan2 = ( 1 - VoH2 ) / VoH2;
	return (chi * 2) / ( 1 + sqrt( 1 + roughness * roughness * tan2 ));*/
}

float	GGX_Distribution(in float NdH, in float roughness)
{
    float r2 = roughness * roughness;
    float NoH2 = NdH * NdH;
    float den = NoH2 * r2 + (1 - NoH2);
    return (chiGGX(NdH) * r2) / ( M_PI * den * den );
}

float Cooktorrance_Specular(in float NdL, in float NdV, in float NdH, in float HdV, in float roughness)
{
	float	D = GGX_Distribution(NdH, roughness);
	float	G = GGX_Geometry(NdV, roughness);
	//float	rim = mix(1.0 - roughness * 0.9, 1.0, NdV);
	return (/*(1 / rim) * */D * G);
}

float D_Phong(in float NdL)
{
    return ((1.0 / M_PI) * NdL);
}

float	Oren_Nayar_Diffuse(in float LdV, in float NdL, in float NdV, in float roughness)
{
	float	lde;
	float	r2;

	lde = LdV - NdL * NdV;
	r2 = roughness * roughness;
	return (clamp(
		max(0.0, NdL) * ((1.0 + r2 * (1 / (r2 + 0.13) + 0.5 / (r2 + 0.33))) +
		(0.45 * r2 / (r2 + 0.09)) * lde / mix(1.0, max(NdL, NdV),
		step(0.0, lde))) / M_PI, 0, 1));
}

vec3 Fresnel_F0(in float ior, in float metallic, in vec3 albedo)
{
	vec3 F0 = vec3(abs((1.0 - ior) / (1.0 + ior)));
	F0 = F0 * F0;
	F0 = mix(F0, albedo, metallic);
	return (F0);
}

vec3 Fresnel_Schlick(in float HdV, in vec3 F0)
{
  return F0 + (1-F0) * pow( 1 - HdV, 5);
}

vec3 fresnel_factor(in vec3 f0, in float product)
{
    return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
}

vec3	light_Pos = vec3(-3, 3, 3);
vec3	light_Color = vec3(1, 1, 1);
float	light_Power = 5;

void main()
{
	vec3 L = normalize(light_Pos - frag_Position);
	vec3 V = normalize(in_CamPos - frag_Position);
	vec3 H = normalize(L + V);
	vec3	albedo = in_Albedo;
	vec3	normal = frag_Normal;
	float	alpha = in_Alpha;
	float	roughness = in_Roughness;
	float	metallic = in_Metallic;

	if (in_Use_Texture_Albedo)
	{
		albedo = texture(in_Texture_Albedo, frag_Texcoord).xyz;
		alpha = texture(in_Texture_Albedo, frag_Texcoord).w;
	}
	if (in_Use_Texture_Normal)
		normal = normalize((texture(in_Texture_Normal, frag_Texcoord).xyz * 2 - 1) * transpose(mat3x3(frag_Tangent, frag_Bitangent, frag_Normal)));
	if (in_Use_Texture_Roughness)
		roughness = texture(in_Texture_Roughness, frag_Texcoord).x;
	if (in_Use_Texture_Metallic)
		metallic = texture(in_Texture_Metallic, frag_Texcoord).x;

	float	light_Attenuation = light_Power * 1.f / (1 + distance(light_Pos, frag_Position));
	light_Color *= light_Attenuation;

	float NdL = max(0.0, dot(normal, L));
	float NdV = max(0.001, dot(normal, V));
	float NdH = max(0.001, dot(normal, H));
	float HdV = max(0.001, dot(H, V));
	float LdV = max(0.001, dot(L, V));

	/*vec3 F0 = Fresnel_F0(in_Refraction, metallic, albedo);
	vec3 fresnel = Fresnel_Schlick(HdV, F0);
	vec3 diffuse = vec3(Oren_Nayar_Diffuse(LdV, NdL, NdV, roughness));

	out_Color = vec4(diffuse, 1);*/

	/*vec3 envspec = textureLod(
		in_Texture_Env_Spec, refl, max(roughness * 10.0, textureQueryLod(in_Texture_Env, refl).y)
	).xyz;*/

	//vec3	c_Specular = mix(vec3(0.04), albedo, metallic);
	vec3	F0 = Fresnel_F0(in_Refraction, metallic, albedo);
	vec3	fresnel = Fresnel_Schlick(NdV, F0);
	vec3	kd = mix(vec3(1.0) - fresnel, vec3(0.04), metallic);

	//vec3	specular = (fresnel * distribution * geometry);
	vec3	specular = fresnel * Cooktorrance_Specular(NdL, NdV, NdH, HdV, roughness);
	vec3	diffuse = kd * albedo * Oren_Nayar_Diffuse(LdV, NdL, NdV, roughness);
	

	vec3	refl = reflect(V, normal);
	vec3	env_diffuse = textureLod(in_Texture_Env, -normal, 10.0).xyz * albedo * kd;
	vec3	env_reflection = textureLod(in_Texture_Env, refl, roughness * 10.f).xyz * fresnel;
	vec3	env_specular = textureLod(in_Texture_Env_Spec, refl, roughness * 10.f).xyz * fresnel;

	//vec3	reflection = mix(albedo, env_reflection, max(0.15, metallic));
	//out_Color = vec4(light_Color, 1);
	out_Color = vec4(env_reflection + env_diffuse + env_specular + light_Color * specular + light_Color * diffuse, 1);
	/*vec3	f_DiffRef = (vec3(1) - fresnel) * Oren_Nayar_Diffuse(LdV, NdL, NdV, roughness);
	vec3	c_RefLight = specular * light_Color;
	vec3	c_DifLight = f_DiffRef * light_Color;
	vec3	iblspec = textureLod(in_Texture_Env_Spec, refl, roughness * 10.f).xyz;//min(vec3(0.99), Fresnel_Schlick(NdV, F0));
	c_RefLight += iblspec;// * envspec;
    c_DifLight += c_Reflection + envdiff * (1.f / M_PI);

	//out_Color = vec4((normal + 1) / 2.f, 1);
	//out_Color = vec4(vec3(GGX_GeometryTerm(NdV, HdV, roughness)), 1);
	out_Color = vec4(c_DifLight * mix(vec3(0.0), albedo, metallic) + c_RefLight, alpha);*/
}
