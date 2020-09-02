R""(
precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

//struct t_Textures {
//#ifdef TEXTURE_USE_ALBEDO
//	sampler2D	Albedo;
//#endif
//#ifdef TEXTURE_USE_SPECULAR
//	sampler2D	Specular;
//#endif
//#ifdef TEXTURE_USE_ROUGHNESS
//	sampler2D	Roughness;
//#endif
//#ifdef TEXTURE_USE_METALLIC
//	sampler2D	Metallic;
//#endif
//#ifdef TEXTURE_USE_METALLICROUGHNESS
//	sampler2D	MetallicRoughness;
//#endif
//#ifdef TEXTURE_USE_EMITTING
//	sampler2D	Emitting;
//#endif
//#ifdef TEXTURE_USE_NORMAL
//	sampler2D	Normal;
//#endif
//#ifdef TEXTURE_USE_HEIGHT
//	sampler2D	Height;
//#endif
//#ifdef TEXTURE_USE_AO
//	sampler2D	AO;
//#endif
//};

//struct t_Material {
//	vec3		Albedo;
//	vec3		Specular;
//	vec3		Emitting;
//	float		Roughness;
//	float		Metallic;
//	float		Alpha;
//	float		Parallax;
//	float		Ior;
//	float		AO;
//};

struct t_Matrix {
	mat4	Model;
	mat4	Normal;
};

struct t_Joint {
	mat4	Matrix;
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

struct	t_Vert {
	vec3	Position;
	vec3	Normal;
	vec2	UV;
};

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec3	in_Tangent;
layout(location = 3) in vec2	in_Texcoord_0;
layout(location = 4) in vec2	in_Texcoord_1;
layout(location = 5) in vec4	in_Color_0;
layout(location = 6) in vec4	in_Joints_0;
layout(location = 7) in vec4	in_Weight_0;

uniform t_Camera				Camera;
uniform t_Matrix				Matrix;
uniform samplerBuffer			Joints;
uniform bool					Skinned;
uniform vec2					UVScale;

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
	Joint[0] = GetJointMatrix(int(in_Joints_0.x));
	Joint[1] = GetJointMatrix(int(in_Joints_0.y));
	Joint[2] = GetJointMatrix(int(in_Joints_0.z));
	Joint[3] = GetJointMatrix(int(in_Joints_0.w));
	if (Skinned) {
		mat4 SkinMatrix =
        in_Weight_0.x * Joint[0] +
        in_Weight_0.y * Joint[1] +
        in_Weight_0.z * Joint[2] +
        in_Weight_0.w * Joint[3];
        mat4 NewModelMatrix = Matrix.Model * SkinMatrix;
        Vert.Position = vec3(NewModelMatrix * vec4(in_Position, 1.0));
        Vert.Normal = mat3(inverse(transpose(NewModelMatrix))) * in_Normal;
	}
	else {
		Vert.Position = vec3(Matrix.Model * vec4(in_Position, 1.0));
		Vert.Normal = mat3(Matrix.Normal) * in_Normal;
	}
	Vert.UV = in_Texcoord_0 * UVScale;
}

void	FillOut()
{
	frag_WorldPosition = Vert.Position;
	frag_WorldNormal = Vert.Normal;
	frag_Texcoord = Vert.UV;
	gl_Position = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Vert.Position, 1);
}

void	ApplyTechnique();

void main()
{
	FillIn();
	ApplyTechnique();
	FillOut();
}

)""