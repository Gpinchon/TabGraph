R""(
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

struct t_Matrix {
	mat4	Model;
	mat4	Normal;
	mat4	ModelViewProjection;
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

struct	t_Vert {
	vec3	Position;
	vec3	Normal;
	vec2	UV;
};

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
uniform t_Camera				Camera;
uniform t_Textures				Texture;
uniform t_Material				Material;
uniform t_Matrix				Matrix;

out vec3						frag_WorldPosition;
out lowp vec3					frag_WorldNormal;
out lowp vec2					frag_Texcoord;

t_Vert	Vert;

void	FillIn()
{
	Vert.Position = vec3(Matrix.Model * vec4(in_Position, 1));
	Vert.Normal = mat3(Matrix.Normal) * in_Normal;//((in_Normal / 255.f) * 2 - 1);
	Vert.UV = in_Texcoord * Texture.Scale;
}

void	FillOut()
{
	frag_WorldPosition = Vert.Position;
	frag_WorldNormal = Vert.Normal;
	frag_Texcoord = Vert.UV;
	gl_Position = Matrix.ModelViewProjection * vec4(in_Position, 1);
}

void	ApplyTechnique();

void main()
{
	FillIn();
	ApplyTechnique();
	FillOut();
}

)""