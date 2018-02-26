#version 410
#define M_PI 3.1415926535897932384626433832795

uniform vec3		in_CamPos;

uniform vec2		in_UVMax;
uniform vec2		in_UVMin;

uniform vec3		in_Albedo;
uniform vec3		in_Emitting;
uniform float		in_Roughness;
uniform float		in_Metallic;
uniform float		in_Refraction;
uniform float		in_Alpha;
uniform float		in_Parallax;
uniform float		in_Stupidity;

uniform bool		in_Use_Texture_Albedo;
uniform bool		in_Use_Texture_Roughness;
uniform bool		in_Use_Texture_Metallic;
uniform bool		in_Use_Texture_Emitting;
uniform bool		in_Use_Texture_Normal;
uniform bool		in_Use_Texture_Height;
uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Roughness;
uniform sampler2D	in_Texture_Metallic;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Height;
uniform sampler2D	in_Texture_Stupid;

uniform sampler2DShadow	in_Texture_Shadow;
uniform samplerCube	in_Texture_Env;
uniform samplerCube	in_Texture_Env_Spec;

in vec3			frag_ShadowPosition;
in vec3			frag_WorldNormal;
in vec3			frag_WorldPosition;
in vec3			frag_ModelNormal;
in vec3			frag_ModelPosition;
in vec2			frag_Texcoord;
in mat4			frag_DepthBiasTransform;

layout(location = 0) out vec4		out_Color;
layout(location = 1) out vec4		out_Bright;
layout(location = 2) out vec4		out_Normal;
layout(location = 3) out vec4		out_Position;

float chiGGX(float v)
{
    return v > 0 ? 1 : 0;
}

float	GGX_Geometry(in float NdV, in float roughness)
{
	float NdV2 = 2 * NdV;
	float r2 = roughness * roughness;
	float denom = NdV + sqrt(r2 + (1 - r2) * (NdV * NdV));
	return (NdV / denom);
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
	float	G = GGX_Geometry(NdL, roughness);
	return (clamp(D * G, 0, 1));
}

float Oren_Nayar_Diffuse(in float LdV, in float NdL, in float NdV, in float roughness)
{
	float s = LdV - NdL * NdV;
	float t = mix(1.0, max(NdL, NdV), step(0.0, s));

	float sigma2 = roughness * roughness;
	float A = 1.0 + sigma2 * (1 / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
	float B = 0.45 * sigma2 / (sigma2 + 0.09);

	return max(0.0, NdL) * (A + B * s / (0.0001 + t)) / M_PI;
}

vec3 Fresnel_F0(in float ior, in float metallic, in vec3 albedo)
{
	vec3	F0 = vec3((ior - 1) / (ior + 1));
	F0 = mix(F0 * F0, albedo * F0, metallic);
	return (F0);
}

vec3 Fresnel_Schlick(in float HdV, in vec3 F0)
{
	float denom = pow(max(0, 1 - HdV), 5);
	return (F0 + (1-F0) * denom);
}

vec3	Fresnel_Schlick_Roughness(in float factor, in vec3 F0, in float roughness)
{
	return (F0 + (max(vec3(1 - roughness), F0) - F0) * pow(1 - factor, 5));
}

vec2 Parallax_Mapping(in vec3 tbnV, in vec2 T, out float parallaxHeight)
{
	const float minLayers = 64;
	const float maxLayers = 128;
	float numLayers = mix(maxLayers, minLayers, abs(tbnV.z));
	int	tries = int(numLayers);

	float layerHeight = 1.0 / numLayers;
	float curLayerHeight = 0;
	vec2 dtex = in_Parallax * tbnV.xy / tbnV.z / numLayers;

	vec2 currentTextureCoords = T;

	float heightFromTexture = 1 - texture(in_Texture_Height, currentTextureCoords).r;

	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		curLayerHeight += layerHeight; 
		currentTextureCoords -= dtex;
		heightFromTexture = 1 - texture(in_Texture_Height, currentTextureCoords).r;
	}


	vec2 prevTCoords = currentTextureCoords + dtex;

	float nextH	= heightFromTexture - curLayerHeight;
	float prevH	= 1 - texture(in_Texture_Height, prevTCoords).r
	- curLayerHeight + layerHeight;

	float weight = nextH / (nextH - prevH);

	vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0 - weight);

	parallaxHeight = (curLayerHeight + prevH * weight + nextH * (1.0 - weight));
	parallaxHeight *= in_Parallax;
	parallaxHeight = isnan(parallaxHeight) ? 0 : parallaxHeight;
	return finalTexCoords;
}

mat3x3	tbn_matrix(in vec3 position, in vec3 normal, in vec2 texcoord)
{
	vec3 Q1 = dFdx(position);
	vec3 Q2 = dFdy(position);
	vec2 st1 = dFdx(texcoord);
	vec2 st2 = dFdy(texcoord);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, normal)));
}

uniform vec3	L = normalize(vec3(-1, 1, 0));
vec3	light_Color = mix(vec3(1), textureLod(in_Texture_Env, -L, 10).rgb, 0.5);

void main()
{
	vec3	worldPosition = frag_WorldPosition;
	vec3	worldNormal = normalize(frag_WorldNormal);
	vec3	shadowPosition = frag_ShadowPosition;
	vec3	albedo = in_Albedo;
	vec3	emitting = in_Emitting;
	vec2	vt = frag_Texcoord;
	vec4	stupid = texture(in_Texture_Stupid, vt);
	float	alpha = in_Alpha;
	float	roughness = in_Roughness;
	float	metallic = in_Metallic;
	mat3x3	tbn;

	if (in_Use_Texture_Height || in_Use_Texture_Normal)
		tbn = tbn_matrix(worldPosition, worldNormal, frag_Texcoord);
	if (in_Use_Texture_Height)
	{
		float ph;
		vt = Parallax_Mapping(tbn * normalize(in_CamPos - worldPosition), vt, ph);
		if(vt.x > in_UVMax.x || vt.y > in_UVMax.y || vt.x < in_UVMin.x || vt.y < in_UVMin.y)
			discard;
		worldPosition = worldPosition - (worldNormal * ph);
		shadowPosition = vec3(frag_DepthBiasTransform * vec4(frag_ModelPosition - (frag_ModelNormal * ph), 1));
	}
	if (in_Use_Texture_Normal)
	{
		vec3 sampleNormal = texture(in_Texture_Normal, vt).xyz * 2 - 1;
		worldNormal = normalize(sampleNormal * tbn);
		if (isnan(length(worldNormal)))
			worldNormal = frag_WorldNormal;
	}
	if (in_Use_Texture_Albedo)
	{
		vec4 col = texture(in_Texture_Albedo, vt);
		albedo = col.rgb;
		alpha = col.a;
	}
	if (alpha <= 0.05f)
		discard;
	if (in_Use_Texture_Roughness)
		roughness = texture(in_Texture_Roughness, vt).x;
	if (in_Use_Texture_Metallic)
		metallic = texture(in_Texture_Metallic, vt).x;
	if (in_Use_Texture_Emitting)
		emitting = texture(in_Texture_Emitting, vt).xyz;
	vec3	V = normalize(in_CamPos - worldPosition);
	vec3	H = normalize(L + V);
	float	light_Power = texture(in_Texture_Shadow, vec3(shadowPosition.xy, shadowPosition.z * 0.995));
	float	NdH = max(0, dot(worldNormal, H));
	float	NdL = max(0, dot(worldNormal, L));
	float	NdV = max(0, abs(dot(worldNormal, V)));
	float	HdV = max(0, dot(H, V));
	float	LdV = max(0, dot(L, V));

	//vec3	F0 = Fresnel_F0(in_Refraction, metallic, albedo);
	vec3	F0 = Fresnel_F0(in_Refraction, metallic, albedo);
	vec3	fresnel = Fresnel_Schlick_Roughness(1 - NdH, F0, roughness);
	vec3	specular = (light_Color * light_Power) * fresnel * Cooktorrance_Specular(NdL, NdV, NdH, HdV, roughness);
	vec3	diffuse = (light_Color * light_Power) * albedo * Oren_Nayar_Diffuse(LdV, NdL, NdV, roughness);

	vec3	refl = reflect(V, worldNormal);
	fresnel = Fresnel_Schlick_Roughness(NdV, F0, roughness);
	vec3	env_diffuse = textureLod(in_Texture_Env, -worldNormal, 10).rgb * albedo;
	vec3	env_diffuse_brightness = textureLod(in_Texture_Env_Spec, -worldNormal, 10).rgb * albedo;
	vec3	env_reflection = textureLod(in_Texture_Env, refl, roughness * 11.f).rgb * fresnel;
	vec3	env_specular = textureLod(in_Texture_Env_Spec, refl, roughness * 11.f).rgb * fresnel;

	vec3	env_color = env_diffuse + env_reflection;
	//env_color /= 1 + albedo;
	vec3	env_brightness = env_diffuse_brightness + env_specular;

	out_Color = vec4(emitting + env_color + env_brightness + specular + diffuse, alpha);
	//out_Color = vec4(fresnel, alpha);
	out_Color = mix(out_Color, stupid, in_Stupidity);
	out_Bright = vec4(max(vec3(0), out_Color.rgb - 0.9) + emitting, alpha);
	out_Normal = vec4(worldNormal, 1);
	out_Position = vec4(worldPosition, 1);
}