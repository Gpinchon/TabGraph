R""(
struct t_Matrix {
	mat4	Model;
	mat4	Normal;
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

layout(location = 0) in vec3	in_Position;

uniform t_Camera		Camera;
uniform t_Matrix		Matrix;
uniform vec3			Resolution;

out VertexData {
	vec2	TexCoord;
	vec3	CubeTexCoord;
} Output;

void TransformGeometry()
{
	gl_Position = Camera.Matrix.Projection * Camera.Matrix.View * Matrix.Model * vec4(in_Position, 1.0);
	Output.TexCoord = gl_Position.xy / vec2(Resolution.xy);
}
)""