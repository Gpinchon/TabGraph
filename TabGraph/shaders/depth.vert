R""(
struct t_Matrix {
	mat4	Model;
};

struct t_CameraMatrix {
	mat4	View;
	mat4	Projection;
	mat4	ViewProjection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
};

layout(location = 0) in vec3	in_Position;
layout(location = 2) in vec2	in_Texcoord;
uniform t_Matrix				Matrix;
uniform t_Camera				Camera;
uniform vec2					UVScale;

out vec2	frag_Texcoord;
void main()
{
	frag_Texcoord = in_Texcoord * UVScale;
	gl_Position = Camera.Matrix.ViewProjection * Matrix.Model * vec4(in_Position, 1);
}

)""