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
	t_Textures	Texture;
};

struct	t_Vert {
	vec3	Position;
	vec3	Normal;
	vec2	UV;
};

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
uniform mat4					in_Transform;
uniform mat4					in_ModelMatrix;
uniform mat4					in_NormalMatrix;
uniform t_Material				Material;

out vec3						frag_WorldPosition;
out lowp vec3					frag_WorldNormal;
out lowp vec2					frag_Texcoord;

t_Vert	Vert;

void	FillIn()
{
	Vert.Position = vec3(in_ModelMatrix * vec4(in_Position, 1));
	Vert.Normal = mat3(in_NormalMatrix) * ((in_Normal / 255.f) * 2 - 1);
	Vert.UV = in_Texcoord * Material.Texture.Scale;
}

void	FillOut()
{
	frag_WorldPosition = Vert.Position;
	frag_WorldNormal = Vert.Normal;
	frag_Texcoord = Vert.UV;
	gl_Position = in_Transform * vec4(in_Position, 1);
}

void	ApplyTechnique();

void main()
{
	FillIn();
	ApplyTechnique();
	FillOut();
}
