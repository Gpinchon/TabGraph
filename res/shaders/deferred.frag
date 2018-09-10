#pragma optionNV (unroll all)
#define M_PI 3.1415926535897932384626433832795

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

#ifdef LIGHTSHADER
struct t_BackTextures {
	sampler2D	Color;
	sampler2D	Emitting;
	sampler2D	Depth;
};
#endif //LIGHTSHADER

struct t_Textures {
	sampler2D		Albedo;
	sampler2D		Emitting;
	sampler2D		Specular;
	sampler2D		MaterialValues;
	sampler2D		AO;
	sampler2D		Normal;
	sampler2D		Depth;
	sampler2D		BRDF;
	t_Environment	Environment;
#ifdef LIGHTSHADER
	t_BackTextures	Back;
#endif //LIGHTSHADER
};

struct t_Material {
	vec3		Albedo;
	vec3		Emitting;
	vec3		Specular;
	float		Roughness;
	float		Metallic;
	float		Ior;
	float		Alpha;
	float		AO;
};

struct t_Frag {
	float		Depth;
	vec2		UV;
	vec3		CubeUV;
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

#ifdef LIGHTSHADER
struct t_Out {
	vec4		Color;
	vec4		Emitting;
};
#endif //LIGHTSHADER

uniform t_Textures		Texture;
uniform t_Camera		Camera;
uniform vec3			Resolution;
uniform float			Time;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

#ifdef POSTSHADER
layout(location = 0) out vec4	out_Albedo;
layout(location = 1) out vec4	out_Emitting;
layout(location = 2) out vec4	out_Fresnel;
layout(location = 3) out vec4	out_Material_Values; //Roughness, Metallic, Ior
layout(location = 4) out vec4	out_AO;
layout(location = 5) out vec4	out_Normal;
#endif //POSTSHADER

#ifdef LIGHTSHADER
layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec4	out_Emitting;
#endif //LIGHTSHADER

t_Frag	Frag;

#ifdef LIGHTSHADER
t_Out	Out;
#endif //LIGHTSHADER

vec3	Position(vec2 UV)
{
	float	linearDepth = texture(Texture.Depth, UV).r * 2.0 - 1.0;
	vec2	coord = UV * 2.0 - 1.0;
	vec4	projectedCoord = vec4(coord, linearDepth, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

vec3	Position()
{
	float	linearDepth = Frag.Depth * 2.0 - 1.0;
	vec2	coord = frag_UV * 2.0 - 1.0;
	vec4	projectedCoord = vec4(coord, linearDepth, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

vec3	OriginalPosition;

void	FillFrag()
{
	Frag.UV = frag_UV;
	Frag.CubeUV = frag_Cube_UV;
	Frag.Depth = gl_FragDepth = texture(Texture.Depth, frag_UV).r;
	OriginalPosition = Frag.Position = Position();
	Frag.Normal = texture(Texture.Normal, frag_UV).xyz;
	vec4	albedo_sample = texture(Texture.Albedo, frag_UV);
	Frag.Material.Albedo = albedo_sample.rgb;
	Frag.Material.Alpha = albedo_sample.a;
	Frag.Material.Specular = texture(Texture.Specular, frag_UV).xyz;
	Frag.Material.Emitting = texture(Texture.Emitting, frag_UV).xyz;
	vec3	MaterialValues = texture(Texture.MaterialValues, frag_UV).xyz;
	Frag.Material.Roughness = MaterialValues.x;
	Frag.Material.Metallic = MaterialValues.y;
	Frag.Material.Ior = MaterialValues.z;
	Frag.Material.AO = texture(Texture.AO, frag_UV).r;
#ifdef LIGHTSHADER
	Out.Color = texture(Texture.Back.Color, frag_UV);
	Out.Emitting = texture(Texture.Back.Emitting, frag_UV);
#endif
}

#ifdef POSTSHADER
void	FillOut()
{
	gl_FragDepth = Frag.Depth;
	bvec3	positionsEqual = notEqual(Frag.Position, OriginalPosition);
	if (positionsEqual.x || positionsEqual.y || positionsEqual.z)
	{
		vec4	NDC = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Frag.Position, 1.0);
		gl_FragDepth = NDC.z / NDC.w * 0.5 + 0.5;
	}
	out_Albedo = vec4(Frag.Material.Albedo, Frag.Material.Alpha);
	out_Fresnel = vec4(Frag.Material.Specular, 1);
	out_Emitting = vec4(Frag.Material.Emitting, 1);
	out_Material_Values = vec4(Frag.Material.Roughness, Frag.Material.Metallic, Frag.Material.Ior, 1);
	out_AO = vec4(Frag.Material.AO, 0, 0, 1);
	out_Normal = vec4(Frag.Normal, 1);
}
#endif

#ifdef LIGHTSHADER
void	FillOut()
{
	out_Color = Out.Color;
	out_Emitting = Out.Emitting;
	gl_FragDepth = Frag.Depth;
}
#endif

vec2	BRDF(in float NdV, in float Roughness)
{
	return (texture(Texture.BRDF, vec2(NdV, Frag.Material.Roughness)).xy);
}

vec4	sampleLod(in samplerCube texture, in vec3 uv, in float value)
{
	return textureLod(texture, uv, value * textureQueryLevels(texture));
}

vec4	sampleLod(in sampler2D texture, in vec2 uv, in float value)
{
	return textureLod(texture, uv, value * textureQueryLevels(texture));
}

float	map(in float value, in float low1, in float high1, in float low2, in float high2)
{
	return (low2 + (value - low1) * (high2 - low2) / (high1 - low1));
}

void	ApplyTechnique();

void main()
{
	FillFrag();
	ApplyTechnique();
	FillOut();
}

