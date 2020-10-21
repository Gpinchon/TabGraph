R""(
layout(location = 0) in vec2	in_Position;

struct t_CameraMatrix {
	mat4	View;
	mat4	Projection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

uniform t_Camera	Camera;

out VertexData {
	vec2	TexCoord;
	vec3	CubeTexCoord;
} Vert;

bool _ClipSpacePositionSet = false;

void SetClipSpacePosition(in vec4 position)
{
	gl_Position = position;
	_ClipSpacePositionSet = true;
}

vec4 ClipSpacePosition()
{
	if (!_ClipSpacePositionSet)
		SetClipSpacePosition(vec4(in_Position, 0, 1));
	return gl_Position;
}

bool _TexCoordSet = false;

void SetTexCoord(in vec2 texCoord)
{
	Vert.TexCoord = texCoord;
	_TexCoordSet = true;
}

vec2 TexCoord()
{
	if (!_TexCoordSet)
		SetTexCoord(vec2(in_Position.x == -1 ? 0 : 1, in_Position.y == -1 ? 0 : 1));
	return Vert.TexCoord;
}

bool _CubeTexCoordSet = false;

void SetCubeTexCoord(in vec3 cubeTexCoord)
{
	Vert.CubeTexCoord = cubeTexCoord;
	_CubeTexCoordSet = true;
}

vec3 CubeTexCoord()
{
	if (!_CubeTexCoordSet)
		SetCubeTexCoord(-mat3(Camera.InvMatrix.View) * (Camera.InvMatrix.Projection * vec4(in_Position, 0, 1)).xyz);
	return Vert.CubeTexCoord;
}

void	FillVertexData()
{
	SetClipSpacePosition(ClipSpacePosition());
	SetTexCoord(TexCoord());
	SetCubeTexCoord(CubeTexCoord());
}

)""