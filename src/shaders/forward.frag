R""(
#define M_PI 3.1415926535897932384626433832795

precision highp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Textures {
	vec2		Scale;
#ifdef TEXTURE_USE_ALBEDO
	sampler2D	Albedo;
#endif
#ifdef TEXTURE_USE_SPECULAR
	sampler2D	Specular;
#endif
#ifdef TEXTURE_USE_ROUGHNESS
	sampler2D	Roughness;
#endif
#ifdef TEXTURE_USE_METALLIC
	sampler2D	Metallic;
#endif
#ifdef TEXTURE_USE_METALLICROUGHNESS
	sampler2D	MetallicRoughness;
#endif
#ifdef TEXTURE_USE_EMITTING
	sampler2D	Emitting;
#endif
#ifdef TEXTURE_USE_NORMAL
	sampler2D	Normal;
#endif
#ifdef TEXTURE_USE_HEIGHT
	sampler2D	Height;
#endif
#ifdef TEXTURE_USE_AO
	sampler2D	AO;
#endif
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

struct t_Matrix {
	mat4	Model;
	mat4	Normal;
	mat4	ViewProjection;
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

struct t_CameraMatrix {
	mat4	View;
	mat4	Projection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

uniform t_Camera		Camera;
uniform t_Textures		Texture;
uniform t_Material		Material;
uniform t_Matrix		Matrix;
uniform t_Environment	Environment;
uniform vec3			Resolution;
uniform float			Time;

in vec3	frag_WorldPosition;
in vec3	frag_WorldNormal;
in vec2	frag_Texcoord;

layout(location = 0) out vec4	out_Albedo;
layout(location = 1) out vec3	out_Emitting;
layout(location = 2) out vec3	out_Specular;
layout(location = 3) out vec3	out_Material_Values; // Roughness, Metallic, Ior
layout(location = 4) out float	out_AO;
layout(location = 5) out vec3	out_Normal;

t_Frag	Frag;

#ifdef TEXTURE_USE_HEIGHT
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
#endif

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

	Frag.Material = Material;
	Frag.Material.Emitting = Material.Emitting;
	Frag.Material.Specular = Material.Specular;
	Frag.Material.Roughness = Material.Roughness;
	Frag.Material.Metallic = Material.Metallic;
	Frag.Material.Ior = Material.Ior;
	Frag.Material.AO = 0;

#ifdef TEXTURE_USE_ALBEDO
	vec4	albedo_sample = texture(Texture.Albedo, Frag.UV);
	Frag.Material.Albedo *= albedo_sample.rgb;
	Frag.Material.Alpha *= albedo_sample.a;
#endif
#ifdef TEXTURE_USE_SPECULAR
	vec3	specular_sample = texture(Texture.Specular, Frag.UV).xyz;
	Frag.Material.Specular = specular_sample;
#endif
#ifdef TEXTURE_USE_ROUGHNESS
	float	roughness_sample = texture(Texture.Roughness, Frag.UV).r;
	Frag.Material.Roughness = roughness_sample;
#endif
#ifdef TEXTURE_USE_METALLIC
	float	metallic_sample = texture(Texture.Metallic, Frag.UV).r;
	Frag.Material.Metallic = metallic_sample;
#endif
#ifdef TEXTURE_USE_METALLICROUGHNESS
	vec2	metallicRoughness_sample = texture(Texture.MetallicRoughness, Frag.UV).bg;
	Frag.Material.Metallic = metallicRoughness_sample.x;
	Frag.Material.Roughness = metallicRoughness_sample.y;
#endif
#ifdef TEXTURE_USE_EMITTING
	vec3	emitting_sample = texture(Texture.Emitting, Frag.UV).rgb;
	Frag.Material.Emitting *= emitting_sample;
#endif
#ifdef TEXTURE_USE_NORMAL
	vec3	normal_sample = texture(Texture.Normal, Frag.UV).xyz * 2 - 1;
	vec3	new_normal = normal_sample * tbn;
	if (dot(new_normal, new_normal) > 0)
		Frag.Normal = new_normal;
#endif
#ifdef TEXTURE_USE_HEIGHT
	Parallax_Mapping(tbn * normalize(Camera.Position - Frag.Position), Frag.UV, ph);
#endif
#ifdef TEXTURE_USE_AO
	Frag.Material.AO = texture(Texture.AO, Frag.UV).r;
#endif
	Frag.Position = Frag.Position - (Frag.Normal * ph);
	Frag.Material.Roughness = map(Frag.Material.Roughness, 0, 1, 0.05, 1);
	Frag.Material.Specular = mix(Frag.Material.Specular, Frag.Material.Albedo.rgb, Frag.Material.Metallic);
}

void	FillOut()
{
	out_Albedo = vec4(Frag.Material.Albedo.rgb, Frag.Material.Alpha);
	out_Specular = Frag.Material.Specular;
	out_Emitting = max(vec3(0), Frag.Material.Albedo.rgb - 1) + Frag.Material.Emitting;
	out_Material_Values.x = Frag.Material.Roughness;
	out_Material_Values.y = Frag.Material.Metallic;
	out_Material_Values.z = Frag.Material.Ior;
	out_AO = Frag.Material.AO;
	out_Normal = normalize(Frag.Normal);
#ifdef FORCEDEPTHWRITE
	gl_FragDepth = Frag.Depth;
	bvec3	positionsEqual = notEqual(Frag.Position, frag_WorldPosition);
	if (positionsEqual.x || positionsEqual.y || positionsEqual.z)
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Frag.Position, 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
#endif //FORCEDEPTHWRITE
}

void	ApplyTechnique();

void	main()
{
	FillIn();
	ApplyTechnique();
	FillOut();
}

)""