#version 410
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

layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec4	out_Bright;
layout(location = 2) out vec4	out_Normal;
layout(location = 3) out vec4	out_Position;

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
	return pow(NdL, 0.5 * (1 - roughness) + 0.5);
}

float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	den = (alpha - 1) + 1;
	float	D = (alpha / (M_PI * den * den));
	float	alpha2 = alpha * alpha;
	float	G = (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
	return (max(D * G, 0));
}

void	main()
{
	out_Color = vec4(0);
	out_Bright = vec4(0);
	out_Normal = vec4(0);
	out_Position = vec4(0);
	vec3	worldPosition = frag_WorldPosition;
	vec3	worldNormal = normalize(frag_WorldNormal);
	mat3x3	tbn;
	vec2	vt = frag_Texcoord;
	vec4	albedo = vec4(Material.Albedo, Material.Alpha);

	tbn = tbn_matrix(frag_WorldPosition, frag_WorldNormal, frag_Texcoord);
	if (Material.Texture.Use_Height)
	{
		float ph;
		Parallax_Mapping(tbn * normalize(in_CamPos - worldPosition), vt, ph);
		worldPosition = worldPosition - (worldNormal * ph);
	}
	vec4	albedo_sample = texture(Material.Texture.Albedo, vt);
	vec3	emitting = texture(Material.Texture.Emitting, vt).rgb + Material.Emitting;
	vec3	normal_sample = texture(Material.Texture.Normal, vt).xyz * 2 - 1;
	vec3	specular_sample = texture(Material.Texture.Specular, vt).xyz;
	float	roughness_sample = texture(Material.Texture.Roughness, vt).r;
	float	metallic_sample = texture(Material.Texture.Metallic, vt).r;
	float	ao = 1 - texture(Material.Texture.AO, vt).r;
	//float	sss = texture(in_Texture_SSS, vt).r;
	
	if (Material.Texture.Use_Albedo)
	{
		albedo.rgb = albedo_sample.rgb;
		albedo.a *= albedo_sample.a;
	}
	if (Material.Texture.Use_Normal)
	{
		vec3	new_normal = normal_sample * tbn;
		if (dot(new_normal, new_normal) > 0)
			worldNormal = normalize(new_normal);
	}
	float	roughness = clamp(Material.Texture.Use_Roughness ? roughness_sample : Material.Roughness, 0.05f, 1.f);
	float	metallic = clamp(Material.Texture.Use_Metallic ? metallic_sample : Material.Metallic, 0.f, 1.f);
	vec3	F0 = mix(Material.Texture.Use_Specular ? specular_sample : Material.Specular, albedo.rgb, metallic);
	vec3	V = normalize(in_CamPos - worldPosition);
	vec3	R = reflect(V, worldNormal);
	float	NdV = max(0, dot(worldNormal, V));

	vec3	fresnel = Fresnel(NdV, F0, roughness);
	vec2	BRDF = texture(Material.Texture.BRDF, vec2(NdV, roughness)).rg;

	vec3	diffuse = ao * (textureLod(Environment.Diffuse, -worldNormal, roughness + 9).rgb
			+ textureLod(Environment.Irradiance, -worldNormal, roughness * 4.f).rgb);
	vec3	reflection = textureLod(Environment.Diffuse, R, roughness * 12.f).rgb * fresnel;
	vec3	specular = textureLod(Environment.Irradiance, R, roughness * 10.f).rgb;
	//vec3	reflection_spec = textureLod(Environment.Diffuse, R, roughness * 10.f + 2.5).rgb;
	vec3	reflection_spec = pow(textureLod(Environment.Diffuse, R, roughness * 10.f + 3.5).rgb, vec3(2.2));

	
	
	if (albedo.a <= 0.05
	|| vt.x > (frag_UVMax.x) || vt.y > (frag_UVMax.y)
	|| vt.x < (frag_UVMin.x) || vt.y < (frag_UVMin.y))
		discard;

	float	brightness = dot(reflection_spec, vec3(0.299, 0.587, 0.114));
	reflection_spec *= brightness * min(fresnel + 1, fresnel * Env_Specular(NdV, roughness));
	specular *= fresnel * BRDF.x + mix(vec3(1), fresnel, metallic) * BRDF.y;
	specular += reflection_spec;
	diffuse *= albedo.rgb * (1 - metallic);
	albedo.a += dot(specular, specular);
	albedo.a = min(1, albedo.a);

	out_Color.rgb += emitting + specular + diffuse + reflection;
	out_Color.a = albedo.a;
	out_Bright = vec4(max(vec3(0), out_Color.rgb - 1) + emitting, albedo.a);
	out_Normal = vec4(worldNormal, 1);
	out_Position = vec4(worldPosition, 1);
}
