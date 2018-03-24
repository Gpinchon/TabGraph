#version 410
#define M_PI 3.1415926535897932384626433832795

uniform vec3		in_CamPos;

uniform vec2		in_UVMax;
uniform vec2		in_UVMin;

uniform vec3		in_Albedo;
uniform vec3		in_Specular;
uniform vec3		in_Emitting;
uniform float		in_Roughness;
uniform float		in_Metallic;
uniform float		in_Alpha;
uniform float		in_Parallax;
uniform float		in_Stupidity;

uniform bool		in_Use_Texture_Albedo = false;
uniform bool		in_Use_Texture_Specular = false;
uniform bool		in_Use_Texture_Roughness = false;
uniform bool		in_Use_Texture_Metallic = false;
uniform bool		in_Use_Texture_Emitting = false;
uniform bool		in_Use_Texture_Normal = false;
uniform bool		in_Use_Texture_Height = false;
uniform bool		in_Use_Texture_AO = false;
uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Specular;
uniform sampler2D	in_Texture_Roughness;
uniform sampler2D	in_Texture_Metallic;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Height;
uniform sampler2D	in_Texture_AO;

uniform sampler2D	in_Texture_Stupid;
uniform sampler2D	in_Texture_BRDF;

uniform sampler2DShadow	in_Texture_Shadow;
uniform samplerCube	in_Texture_Env;
uniform samplerCube	in_Texture_Env_Spec;

uniform vec3		L = normalize(vec3(-1, 1, 0));

in vec3			frag_ShadowPosition;
in vec3			frag_WorldNormal;
in vec3			frag_WorldPosition;
in vec3			frag_ModelNormal;
in vec3			frag_ModelPosition;
in vec2			frag_Texcoord;

layout(location = 0) out vec4		out_Color;
layout(location = 1) out vec4		out_Bright;
layout(location = 2) out vec4		out_Normal;
layout(location = 3) out vec4		out_Position;

float	GGX_Geometry(in float NdV, in float NdL, in float alpha)
{
	float	alpha2 = alpha * alpha;
	return (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
}

float	GGX_Distribution(in float NdH, in float alpha)
{
	float den = (NdH * NdH) * (alpha - 1) + 1;
	return (alpha / (M_PI * den * den));
}

float Specular(in float NdL, in float NdV, in float NdH, in float HdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	D = GGX_Distribution(NdH, alpha);
	float	G = GGX_Geometry(NdV, NdL, alpha);
	return (max(D * G, 0));
}

vec3	Fresnel_Schlick_Roughness(in float factor, in vec3 F0, in float roughness)
{
	return (F0 + (max(vec3(1 - roughness), F0) - F0) * pow(max(0, 1 - factor), 5));
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

float	CustomLambertianDiffuse(float NdL, float roughness)
{
	return pow(NdL, 0.5 + 0.5 * (1 - roughness));
}

void main()
{
	vec3	worldPosition = frag_WorldPosition;
	vec3	worldNormal = normalize(frag_WorldNormal);
	mat3x3	tbn;
	vec2	vt = frag_Texcoord;
	vec4	albedo = vec4(in_Albedo, in_Alpha);
	vec3	emitting = in_Emitting;
	vec3	F0 = in_Specular;
	float	roughness = in_Roughness;
	float	metallic = in_Metallic;
	float	ao = 1;

	vec3	light_Color = (vec3(0.5) + texture(in_Texture_Env_Spec, -L).rgb) * 0.5;
	tbn = tbn_matrix(frag_WorldPosition, frag_WorldNormal, frag_Texcoord);
	if (in_Use_Texture_Height)
	{
		float ph;
		vt = Parallax_Mapping(tbn * normalize(in_CamPos - worldPosition), vt, ph);
		worldPosition = worldPosition - (worldNormal * ph);
	}
	vec4	albedo_sample = texture(in_Texture_Albedo, vt);
	vec3	emitting_sample = texture(in_Texture_Emitting, vt).rgb;
	vec3	normal_sample = texture(in_Texture_Normal, vt).xyz * 2 - 1;
	vec3	specular_sample = texture(in_Texture_Specular, vt).xyz;
	float	roughness_sample = texture(in_Texture_Roughness, vt).r;
	float	metallic_sample = texture(in_Texture_Metallic, vt).r;
	float	ao_sample = texture(in_Texture_AO, vt).r;
	vec4	stupid_sample = texture(in_Texture_Stupid, vt);
	float	light_Power = texture(in_Texture_Shadow, vec3(frag_ShadowPosition.xy, frag_ShadowPosition.z * 0.995));

	if (in_Use_Texture_Normal)
	{
		vec3	new_normal = normal_sample * tbn;
		if (dot(new_normal, new_normal) > 0)
			worldNormal = normalize(new_normal);
	}
	if (in_Use_Texture_Albedo)
	{
		albedo.rgb = albedo_sample.rgb;
		albedo.a = albedo_sample.a;
	}
	if (step(0.05, albedo.a) == 0 || vt.x > in_UVMax.x || vt.y > in_UVMax.y || vt.x < in_UVMin.x || vt.y < in_UVMin.y)
		discard;
	if (in_Use_Texture_Specular)
		F0 = specular_sample;
	if (in_Use_Texture_Roughness)
		roughness = roughness_sample;
	if (in_Use_Texture_Metallic)
		metallic = metallic_sample;
	if (in_Use_Texture_AO)
		ao = ao_sample;
	if (in_Use_Texture_Emitting)
		emitting = emitting_sample;
	F0 = mix(F0, albedo.rgb, metallic);
	vec3	V = normalize(in_CamPos - worldPosition);
	vec3	H = normalize(L + V);
	vec3	R = reflect(V, worldNormal);
	vec3	env_diffuse =	textureLod(in_Texture_Env, -worldNormal, roughness + 9).rgb;
		+ textureLod(in_Texture_Env_Spec, -worldNormal, roughness * 4.f).rgb;
	vec3	env_reflection = textureLod(in_Texture_Env, R, roughness * 11.f).rgb;
	vec3	env_reflection_spec = textureLod(in_Texture_Env, R, roughness * 11.f + 2.5).rgb;
	vec3	env_specular = textureLod(in_Texture_Env_Spec, R, roughness * 4.f).rgb;

	float	NdH = max(0, dot(worldNormal, H));
	float	NdL = max(0, dot(worldNormal, L));
	float	NdV = max(0, dot(worldNormal, V));
	float	HdV = max(0, dot(H, V));
	float	LdV = max(0, dot(L, V));

	vec3	fresnel = Fresnel_Schlick_Roughness(NdH, F0, roughness);
	albedo.rgb *= (1 - metallic);
	light_Color *= light_Power;
	vec3	specular = light_Color * fresnel * Specular(NdL, NdV, NdH, HdV, roughness);
	vec3	diffuse = light_Color * albedo.rgb * CustomLambertianDiffuse(NdL, roughness);

	fresnel = Fresnel_Schlick_Roughness(NdV, F0, roughness);
	vec2	BRDF = texture(in_Texture_BRDF, vec2(NdV, roughness)).rg;
	
	env_diffuse *= albedo.rgb;
	env_reflection_spec = pow(env_reflection_spec, vec3(2.2));
	env_reflection_spec = (1 - fresnel) * (1 - roughness) * env_reflection_spec * dot(env_reflection_spec, vec3(0.299, 0.587, 0.114));
	env_reflection = env_reflection * fresnel;
	env_specular *= fresnel * BRDF.x + mix(vec3(1), fresnel, metallic) * BRDF.y;
	env_specular += env_reflection_spec;

	diffuse += env_diffuse + env_reflection;
	specular += env_specular;

	albedo.a += dot(specular, specular);
	albedo.a = min(1, albedo.a);
	out_Color.rgb = emitting + specular + diffuse;
	//out_Color.rgb = env_diffuse + env_reflection + env_specular;
	//out_Color.rgb = env_reflection_spec;
	out_Color.a = albedo.a;
	out_Color = mix(out_Color, stupid_sample, in_Stupidity);
	out_Bright = vec4(max(vec3(0), out_Color.rgb - 1) + emitting, albedo.a);
	out_Normal = vec4(worldNormal, 1);
	out_Position = vec4(worldPosition, 1);
}
