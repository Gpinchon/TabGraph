R""(
precision lowp float;
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

struct t_Joint {
	mat4	Matrix;
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
layout(location = 3) in vec2	in_Texcoord1;
layout(location = 4) in vec4	in_Color;
layout(location = 5) in vec4	in_Joint;
layout(location = 6) in vec4	in_Weight;

uniform t_Textures				Texture;
uniform t_Matrix				Matrix;
uniform samplerBuffer			Joints;
uniform bool					Skinned;

out vec3						frag_WorldPosition;
out lowp vec3					frag_WorldNormal;
out lowp vec2					frag_Texcoord;

t_Vert	Vert;

mat4	GetJointMatrix(int index)
{
	return mat4(
		texelFetch(Joints, index * 4 + 0),
		texelFetch(Joints, index * 4 + 1),
		texelFetch(Joints, index * 4 + 2),
		texelFetch(Joints, index * 4 + 3)
	);
}

void	FillIn()
{
	mat4 Joint[4];
	Joint[0] = GetJointMatrix(int(in_Joint.x));
	Joint[1] = GetJointMatrix(int(in_Joint.y));
	Joint[2] = GetJointMatrix(int(in_Joint.z));
	Joint[3] = GetJointMatrix(int(in_Joint.w));
	if (Skinned) {
		mat4 SkinMatrix =
        in_Weight.x * Joint[0] +
        in_Weight.y * Joint[1] +
        in_Weight.z * Joint[2] +
        in_Weight.w * Joint[3];
        Vert.Position = vec3(Matrix.Model * SkinMatrix * vec4(in_Position, 1.0));
	}
	else
   		Vert.Position = vec3(Matrix.Model * vec4(in_Position, 1.0));
	Vert.Normal = mat3(Matrix.Normal) * in_Normal;
	Vert.UV = in_Texcoord * Texture.Scale;
}

void	FillOut()
{
	frag_WorldPosition = Vert.Position;
	frag_WorldNormal = Vert.Normal;
	frag_Texcoord = Vert.UV;
	gl_Position = Matrix.ViewProjection * vec4(Vert.Position, 1);
}

void	ApplyTechnique();

void main()
{
	FillIn();
	ApplyTechnique();
	FillOut();
}

)""