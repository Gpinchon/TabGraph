R""(
#define M_PI 3.1415926535897932384626433832795
#if defined(TEXTURE_USE_HEIGHT) || defined(TEXTURE_USE_DIFFUSE) || defined(TEXTURE_USE_EMITTING) || defined(TEXTURE_USE_NORMAL) || defined(TEXTURE_USE_AO)
#define USE_TEXTURES
#endif

precision highp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Matrix {
	mat4	Model;
	mat4	Normal;
};

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

#ifdef USE_TEXTURES
struct t_StandardTextures {
#ifdef TEXTURE_USE_HEIGHT
	sampler2D	Height;
#endif
#ifdef TEXTURE_USE_DIFFUSE
	sampler2D	Diffuse;
#endif
#ifdef TEXTURE_USE_EMITTING
	sampler2D	Emitting;
#endif
#ifdef TEXTURE_USE_NORMAL
	sampler2D	Normal;
#endif
#ifdef TEXTURE_USE_AO
	sampler2D	AO;
#endif
};
#endif

struct t_StandardValues {
	vec3		Diffuse;
	//vec3		Specular;
	vec3		Emitting;
	float		Opacity;
	float		Parallax;
	float		Ior;
	float		AO;
};

struct t_BRDF {
	vec3	CDiff;
	vec3	F0;
	float	Alpha;
};

struct t_Frag {
	float		Depth;
	vec2		UV;
	vec3		Position;
	vec3		Normal;
	t_BRDF		BRDF;
};

struct t_CameraMatrix {
	mat4	View;
	mat4	Projection;
	mat4	ViewProjection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

uniform t_Camera			Camera;
#ifdef USE_TEXTURES
uniform t_StandardTextures	StandardTextures;
#endif
uniform t_StandardValues	_StandardValues;
uniform t_Matrix			Matrix;
uniform t_Environment		Environment;
uniform vec3				Resolution;
uniform float				Time;
uniform vec2				UVScale;

t_StandardValues StandardValues;

in vec3	frag_WorldPosition;
in vec3	frag_WorldNormal;
in vec2	frag_Texcoord;

layout(location = 0) out vec4	out_CDiff;
layout(location = 1) out vec3	out_Emitting;
layout(location = 2) out vec3	out_F0;
layout(location = 3) out vec3	out_Material_Values; // BRDF Alpha, Metallic, Ior
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
	vec2 dtex = StandardMaterial.Parallax * tbnV.xy / tbnV.z / numLayers;
	vec2 currentTextureCoords = T;
	float heightFromTexture = 1 - texture(StandardTextures.Height, currentTextureCoords).r;
	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		curLayerHeight += layerHeight; 
		currentTextureCoords -= dtex;
		heightFromTexture = 1 - texture(StandardTextures.Height, currentTextureCoords).r;
	}
	vec2 prevTCoords = currentTextureCoords + dtex;
	float nextH	= heightFromTexture - curLayerHeight;
	float prevH	= 1 - texture(StandardTextures.Height, prevTCoords).r
	- curLayerHeight + layerHeight;
	float weight = nextH / (nextH - prevH);
	vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0 - weight);
	parallaxHeight = (curLayerHeight + prevH * weight + nextH * (1.0 - weight));
	parallaxHeight *= StandardMaterial.Parallax;
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
	//Frag.Material = Material;
	//Frag.Material.Emitting = Material.Emitting;
	//Frag.Material.Specular = Material.Specular;
	//MetallicRoughness.Value.Roughness = Material.Roughness;
	//MetallicRoughness.Value.Metallic = Material.Metallic;
	//Frag.Material.Ior = Material.Ior;
	//Frag.Material.AO = 0;
	Frag.Position = frag_WorldPosition;
	Frag.Normal = normalize(frag_WorldNormal);
	Frag.UV = frag_Texcoord * UVScale;
	Frag.Depth = gl_FragCoord.z;
	StandardValues = _StandardValues;
#if defined(TEXTURE_USE_HEIGHT) || defined(TEXTURE_USE_NORMAL)
	mat3	tbn = tbn_matrix();
#endif
//#ifdef USE_TEXTURES
//	StandardTextures = StandardTextures;
//#endif
#ifdef TEXTURE_USE_HEIGHT
	float ph = 0;
	Parallax_Mapping(tbn * viewDir, Frag.UV, ph);
#endif
#ifdef TEXTURE_USE_DIFFUSE
	vec4	albedo_sample = texture(StandardTextures.Diffuse, Frag.UV);
	StandardValues.Diffuse *= albedo_sample.rgb;
	StandardValues.Opacity *= albedo_sample.a;
#endif
#ifdef TEXTURE_USE_EMITTING
	StandardValues.Emitting *= texture(StandardTextures.Emitting, Frag.UV).rgb;
#endif
#ifdef TEXTURE_USE_AO
	StandardValues.AO = texture(StandardTextures.AO, Frag.UV).r;
#endif
#ifdef TEXTURE_USE_NORMAL
	vec3	normal_sample = texture(StandardTextures.Normal, Frag.UV).xyz * 2 - 1;
	vec3	new_normal = normal_sample * tbn;
	if (dot(new_normal, new_normal) > 0)
		Frag.Normal = new_normal;
#endif
#ifdef TEXTURE_USE_HEIGHT
	Frag.Position = Frag.Position - (Frag.Normal * ph);
#endif
}

void	FillOut()
{
	out_CDiff = vec4(Frag.BRDF.CDiff, StandardValues.Opacity);
	out_F0 = Frag.BRDF.F0;
	out_Emitting = max(vec3(0), StandardValues.Emitting + StandardValues.Diffuse.rgb - 1);
	out_Material_Values.x = Frag.BRDF.Alpha;
	//out_Material_Values.y = StandardValues.Metallic;
	out_Material_Values.z = StandardValues.Ior;
	out_AO = StandardValues.AO;
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