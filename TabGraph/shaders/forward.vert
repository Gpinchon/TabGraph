R""(
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
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

/*struct	t_Vert {
	vec3	Position;
	vec3	Normal;
	vec2	UV;
};*/

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec3	in_Tangent;
layout(location = 3) in vec2	in_TexCoord_0;
layout(location = 4) in vec2	in_TexCoord_1;
layout(location = 5) in vec4	in_Color_0;
layout(location = 6) in vec4	in_Joints_0;
layout(location = 7) in vec4	in_Weight_0;

uniform t_Camera				PrevCamera;
uniform t_Camera				Camera;
uniform t_Matrix				PrevMatrix;
uniform t_Matrix				Matrix;
uniform samplerBuffer			Joints;
uniform bool					Skinned;
uniform vec2					UVScale;

out VertexData {
	vec3	WorldPosition;
	vec3	WorldNormal;
	vec2	TexCoord;
} Output;

out vec4 Position;
out vec4 PreviousPosition;

out float CameraSpaceDepth;

bool _WorldPositionSet = false;

void SetWorldPosition(in vec3 worldPosition)
{
	Output.WorldPosition = worldPosition;
	_WorldPositionSet = true;
}

vec3 WorldPosition()
{
	if (!_WorldPositionSet)
		SetWorldPosition(vec3(Matrix.Model * vec4(in_Position, 1.0)));
	return Output.WorldPosition;
}

bool _WorldNormalSet = false;

void SetWorldNormal(in vec3 worldNormal)
{
	Output.WorldNormal = worldNormal;
	_WorldNormalSet = true;
}

vec3 WorldNormal()
{
	if (!_WorldNormalSet)
		SetWorldNormal(mat3(Matrix.Normal) * in_Normal);
	return Output.WorldNormal;
}

bool _TexCoordSet = false;

void SetTexCoord(in vec2 texCoord)
{
	Output.TexCoord = texCoord;
	_TexCoordSet = true;
}

vec2 TexCoord()
{
	if (!_TexCoordSet)
		SetTexCoord(in_TexCoord_0);
	return Output.TexCoord;
}

bool _ClipSpacePositionSet = false;

void SetClipSpacePosition(in vec4 clipSpacePosition)
{
	gl_Position = clipSpacePosition;
	_ClipSpacePositionSet = true;
}

vec4 ClipSpacePosition()
{
	if (!_ClipSpacePositionSet)
		SetClipSpacePosition(Camera.Matrix.Projection * Camera.Matrix.View * vec4(WorldPosition(), 1));
	return gl_Position;
}

mat4	GetJointMatrix(int index)
{
	return mat4(
		texelFetch(Joints, index * 4 + 0),
		texelFetch(Joints, index * 4 + 1),
		texelFetch(Joints, index * 4 + 2),
		texelFetch(Joints, index * 4 + 3)
	);
}

void	FillVertexData()
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
		SetWorldPosition(vec3(NewModelMatrix * vec4(in_Position, 1.0)));
		SetWorldNormal(mat3(inverse(transpose(NewModelMatrix))) * in_Normal);
	}
	else {
		SetWorldPosition(WorldPosition());
		SetWorldNormal(WorldNormal());
	}
	
	SetTexCoord(TexCoord() * UVScale);
	vec4 cameraSpacePosition = Camera.Matrix.View * vec4(WorldPosition(), 1);
	CameraSpaceDepth = cameraSpacePosition.z;
	SetClipSpacePosition(Position = (Camera.Matrix.Projection * cameraSpacePosition));
	PreviousPosition = PrevCamera.Matrix.Projection * PrevCamera.Matrix.View * PrevMatrix.Model * vec4(in_Position, 1.0);
}

/*void	FillOut()
{
	frag_WorldPosition = Output.Position;
	frag_WorldNormal = Output.Normal;
	frag_TexCoord = Output.UV;
	gl_Position = Camera.Matrix.Projection * Camera.Matrix.View * vec4(Output.Position, 1);
}*/

/*void	ApplyTechnique();

void main()
{
	FillIn();
	ApplyTechnique();
	FillOut();
}*/

)""