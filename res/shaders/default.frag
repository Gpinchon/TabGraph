#version 450
#define M_PI 3.1415926535897932384626433832795

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Textures {
	vec2		Scale;
	sampler2D	Albedo;
	bool		Use_Albedo;
	sampler2D	Specular;
	bool		Use_Specular;
	sampler2D	Roughness;
	bool		Use_Roughness;
	sampler2D	Metallic;
	bool		Use_Metallic;
	sampler2D	Emitting;
	bool		Use_Normal;
	sampler2D	Normal;
	bool		Use_Height;
	sampler2D	Height;
	sampler2D	AO;
	sampler2D	BRDF;
};

struct t_Material {
	vec3		Albedo;
	vec3		Specular;
	vec3		Emitting;
	float		Roughness;
	float		Metallic;
	float		Alpha;
	float		Parallax;
	float		Ior;
	t_Textures	Texture;
};

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

uniform t_Material		Material;
uniform t_Environment	Environment;
uniform vec3			in_CamPos;

in vec3	frag_WorldPosition;
in vec3	frag_WorldNormal;
in vec2	frag_Texcoord;
in vec2	frag_UVMax;
in vec2	frag_UVMin;

layout(location = 0) out vec4	out_Albedo; // Albedo;
layout(location = 1) out vec4	out_Emitting; // Emitting;
layout(location = 2) out vec4	out_Fresnel; // Fresnel;
layout(location = 3) out vec4	out_Material_Values; // Material_Values -> Roughness, Metallic, Ior
layout(location = 4) out vec4	out_BRDF;// BRDF
layout(location = 5) out vec4	out_AO;//AO
layout(location = 6) out vec4	out_Normal;// Normal;
layout(location = 7) out vec4	out_Position;// Position;

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

vec3	Fresnel(in float factor, in vec3 F0, in float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
}

void	Parallax_Mapping(in vec3 tbnV, inout vec2 T, out float parallaxHeight)
{
	const float minLayers = 10;
	const float maxLayers = 15;
	float numLayers = mix(maxLayers, minLayers, abs(tbnV.z));
	int	tries = int(numLayers);
	float layerHeight = 1.0 / numLayers;
	float curLayerHeight = 0;
	vec2 dtex = Material.Parallax * tbnV.xy / tbnV.z / numLayers;
	vec2 currentTextureCoords = T;
	float heightFromTexture = 1 - texture(Material.Texture.Height, currentTextureCoords).r;
	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		curLayerHeight += layerHeight; 
		currentTextureCoords -= dtex;
		heightFromTexture = 1 - texture(Material.Texture.Height, currentTextureCoords).r;
	}
	vec2 prevTCoords = currentTextureCoords + dtex;
	float nextH	= heightFromTexture - curLayerHeight;
	float prevH	= 1 - texture(Material.Texture.Height, prevTCoords).r
	- curLayerHeight + layerHeight;
	float weight = nextH / (nextH - prevH);
	vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0 - weight);
	parallaxHeight = (curLayerHeight + prevH * weight + nextH * (1.0 - weight));
	parallaxHeight *= Material.Parallax;
	parallaxHeight = isnan(parallaxHeight) ? 0 : parallaxHeight;
	T = finalTexCoords;
}

mat3x3	tbn_matrix(in vec3 position, in vec3 normal, in vec2 texcoord)
{
	vec3 Q1 = dFdxCoarse(position);
	vec3 Q2 = dFdyCoarse(position);
	vec2 st1 = dFdxCoarse(texcoord);
	vec2 st2 = dFdyCoarse(texcoord);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, normal)));
}

void	main()
{
	vec3	worldPosition = frag_WorldPosition;
	vec3	worldNormal = normalize(frag_WorldNormal);
	mat3x3	tbn;
	vec2	vt = frag_Texcoord;
	vec4	albedo = vec4(Material.Albedo, Material.Alpha);

	tbn = tbn_matrix(frag_WorldPosition, frag_WorldNormal, frag_Texcoord);
	vec3	V = normalize(in_CamPos - worldPosition);
	if (Material.Texture.Use_Height)
	{
		float ph;
		Parallax_Mapping(tbn * V, vt, ph);
		worldPosition = worldPosition - (worldNormal * ph);
	}
	vec4	albedo_sample = texture(Material.Texture.Albedo, vt);
	vec3	emitting = texture(Material.Texture.Emitting, vt).rgb + Material.Emitting;
	vec3	normal_sample = texture(Material.Texture.Normal, vt).xyz * 2 - 1;
	vec3	specular_sample = texture(Material.Texture.Specular, vt).xyz;
	float	roughness_sample = texture(Material.Texture.Roughness, vt).r;
	float	metallic_sample = texture(Material.Texture.Metallic, vt).r;
	float	ao = texture(Material.Texture.AO, vt).r;
	
	if (Material.Texture.Use_Albedo) {
		albedo *= albedo_sample;
	}
	if (albedo.a <= 0.01
	|| vt.x > (frag_UVMax.x) || vt.y > (frag_UVMax.y)
	|| vt.x < (frag_UVMin.x) || vt.y < (frag_UVMin.y))
		discard;
	if (Material.Texture.Use_Normal)
	{
		vec3	new_normal = normal_sample * tbn;
		if (dot(new_normal, new_normal) > 0)
			worldNormal = new_normal;
	}
	float	roughness = max(0.01, Material.Texture.Use_Roughness ? roughness_sample : Material.Roughness);
	float	metallic = Material.Texture.Use_Metallic ? metallic_sample : Material.Metallic;
	vec3	F0 = mix(Material.Texture.Use_Specular ? specular_sample : Material.Specular, albedo.rgb, metallic);
	float	NdV = dot(worldNormal, V);
	if (albedo.a < 1 && NdV < 0)
		NdV = -NdV;
	else if (NdV < 0)
		NdV = 0;

	vec3	fresnel = Fresnel(NdV, F0, roughness);
	vec2	BRDF = texture(Material.Texture.BRDF, vec2(NdV, roughness)).rg;

	out_Albedo.a = 1;
	out_Fresnel.a = 1;
	out_Emitting.a = 1;
	out_Material_Values.a = 1;
	out_BRDF.a = 1;
	out_AO.a = 1;
	out_Normal.a = 1;
	out_Position.a = 1;

	out_Albedo = vec4(albedo.rgb, albedo.a);
	out_Fresnel.rgb = fresnel;
	out_Emitting.rgb = max(vec3(0), albedo.rgb - 1) + emitting;
	out_Material_Values.x = roughness;
	out_Material_Values.y = metallic;
	out_Material_Values.z = Material.Ior;
	out_BRDF.xy = BRDF;
	out_AO.r = ao;
	out_Normal.xyz = normalize(worldNormal);
	out_Position.xyz = worldPosition;
}
