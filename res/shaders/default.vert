#version 450

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

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
uniform mat4					in_Transform;
uniform mat4					in_ModelMatrix;
uniform mat4					in_NormalMatrix;
uniform lowp vec2				in_UVMax;
uniform lowp vec2				in_UVMin;
uniform t_Material				Material;

out vec3						frag_WorldPosition;
out lowp vec3					frag_WorldNormal;
out lowp vec2					frag_Texcoord;
out lowp vec2					frag_UVMax;
out lowp vec2					frag_UVMin;

void main()
{
	frag_WorldPosition = vec3(in_ModelMatrix * vec4(in_Position, 1));
	frag_WorldNormal = mat3(in_NormalMatrix) * ((in_Normal / 255.f) * 2 - 1);
	frag_Texcoord = in_Texcoord * Material.Texture.Scale;
	frag_UVMax = in_UVMax * Material.Texture.Scale;
	frag_UVMin = in_UVMin * Material.Texture.Scale;
	gl_Position = in_Transform * vec4(in_Position, 1);
}
