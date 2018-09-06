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
	float		AO;
};

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

struct t_Frag {
	float		Depth;
	vec2		UV;
	vec3		Position;
	vec3		Normal;
	t_Material	Material;
};

uniform t_Textures		Texture;
uniform t_Material		Material;
uniform t_Environment	Environment;
uniform vec3			in_CamPos;

in vec3	frag_WorldPosition;
in vec3	frag_WorldNormal;
in vec2	frag_Texcoord;
in vec2	frag_UVMax;
in vec2	frag_UVMin;

layout(location = 0) out vec4	out_Albedo;
layout(location = 1) out vec4	out_Emitting;
layout(location = 2) out vec4	out_Fresnel;
layout(location = 3) out vec4	out_Material_Values; // Roughness, Metallic, Ior
layout(location = 4) out vec4	out_AO;
layout(location = 5) out vec4	out_Normal;

t_Frag	Frag;

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
	float heightFromTexture = 1 - texture(Texture.Height, currentTextureCoords).r;
	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		curLayerHeight += layerHeight; 
		currentTextureCoords -= dtex;
		heightFromTexture = 1 - texture(Texture.Height, currentTextureCoords).r;
	}
	vec2 prevTCoords = currentTextureCoords + dtex;
	float nextH	= heightFromTexture - curLayerHeight;
	float prevH	= 1 - texture(Texture.Height, prevTCoords).r
	- curLayerHeight + layerHeight;
	float weight = nextH / (nextH - prevH);
	vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0 - weight);
	parallaxHeight = (curLayerHeight + prevH * weight + nextH * (1.0 - weight));
	parallaxHeight *= Material.Parallax;
	parallaxHeight = isnan(parallaxHeight) ? 0 : parallaxHeight;
	T = finalTexCoords;
}

mat3x3	tbn_matrix()
{
	vec3 Q1 = dFdx(Frag.Position);
	vec3 Q2 = dFdy(Frag.Position);
	vec2 st1 = dFdx(Frag.UV);
	vec2 st2 = dFdy(Frag.UV);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, Frag.Normal)));
}

float	map(in float value, in float low1, in float high1, in float low2, in float high2)
{
	return (low2 + (value - low1) * (high2 - low2) / (high1 - low1));
}

void	FillIn()
{
	Frag.Position = frag_WorldPosition;
	Frag.Normal = normalize(frag_WorldNormal);
	Frag.UV = frag_Texcoord;
	Frag.Depth = gl_FragCoord.z;

	mat3	tbn = tbn_matrix();
	float	ph = 0;

	if (Texture.Use_Height)
		Parallax_Mapping(tbn * normalize(in_CamPos - Frag.Position), Frag.UV, ph);

	Frag.Material = Material;
	Frag.Material.Emitting = texture(Texture.Emitting, Frag.UV).rgb + Material.Emitting;
	Frag.Material.AO = texture(Texture.AO, Frag.UV).r;
	Frag.Material.Specular = Material.Specular;
	Frag.Material.Roughness = Material.Roughness;
	Frag.Material.Metallic = Material.Metallic;
	Frag.Material.Ior = Material.Ior;

	vec4	albedo_sample = texture(Texture.Albedo, Frag.UV);
	vec3	normal_sample = texture(Texture.Normal, Frag.UV).xyz * 2 - 1;
	vec3	specular_sample = texture(Texture.Specular, Frag.UV).xyz;
	float	roughness_sample = texture(Texture.Roughness, Frag.UV).r;
	float	metallic_sample = texture(Texture.Metallic, Frag.UV).r;

	if (Texture.Use_Albedo)
	{
		Frag.Material.Albedo *= albedo_sample.rgb;
		Frag.Material.Alpha *= albedo_sample.a;
	}
	if (Texture.Use_Specular)
		Frag.Material.Specular = specular_sample;
	if (Texture.Use_Roughness)
		Frag.Material.Roughness = roughness_sample;
	if (Texture.Use_Metallic)
		Frag.Material.Metallic = metallic_sample;
	if (Texture.Use_Normal) {
		vec3	new_normal = normal_sample * tbn;
		if (dot(new_normal, new_normal) > 0)
			Frag.Normal = new_normal;
	}
	if (Texture.Use_Height)
		Frag.Position = Frag.Position - (Frag.Normal * ph);

	Frag.Material.Roughness = map(Frag.Material.Roughness, 0, 1, 0.1, 1);
	Frag.Material.Specular = mix(Frag.Material.Specular, Frag.Material.Albedo.rgb, Frag.Material.Metallic);
}

/* vec3	Fresnel(in float factor, in vec3 F0, in float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
} */

void	FillOut()
{
	//float	NdV = dot(Frag.Normal, normalize(in_CamPos - Frag.Position));

	/* if (Frag.Material.Alpha < 1 && NdV < 0)
		NdV = -NdV;
	else if (NdV < 0)
		NdV = 0; */
	out_Fresnel.a = 1;
	out_Emitting.a = 1;
	out_Material_Values.a = 1;
	out_AO.a = 1;
	out_Normal.a = 1;

	out_Albedo = vec4(Frag.Material.Albedo.rgb + Frag.Material.Emitting, Frag.Material.Alpha);
	//out_Fresnel.rgb = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);
	out_Fresnel.rgb = Frag.Material.Specular;
	out_Emitting.rgb = max(vec3(0), Frag.Material.Albedo.rgb - 1) + Frag.Material.Emitting;
	out_Material_Values.x = Frag.Material.Roughness;
	out_Material_Values.y = Frag.Material.Metallic;
	out_Material_Values.z = Frag.Material.Ior;
	out_AO.r = Frag.Material.AO;
	out_Normal.xyz = normalize(Frag.Normal);
}

void	ApplyTechnique();

void	main()
{
	FillIn();
	ApplyTechnique();
	FillOut();
}
