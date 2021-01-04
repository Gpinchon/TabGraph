R""(
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

void SetClipSpacePosition(in vec4 position)
{
	gl_Position = position;
}

vec4 ClipSpacePosition()
{
	return gl_Position;
}

void SetTexCoord(in vec2 texCoord)
{
	Vert.TexCoord = texCoord;
}

vec2 TexCoord()
{
	return Vert.TexCoord;
}

void SetCubeTexCoord(in vec3 cubeTexCoord)
{
	Vert.CubeTexCoord = cubeTexCoord;
}

vec3 CubeTexCoord()
{
	return Vert.CubeTexCoord;
}

void	FillVertexData()
{
	float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
	SetTexCoord(vec2((x+1.0)*0.5, (y+1.0)*0.5));
    SetClipSpacePosition(vec4(x, y, 0, 1));
	SetCubeTexCoord(-mat3(Camera.InvMatrix.View) * (Camera.InvMatrix.Projection * gl_Position).xyz);
}

)""