struct t_Matrix {
	mat4	Model;
	mat4	Normal;
	mat4	ModelViewProjection;
};

layout(location = 0) in vec3	in_Position;
layout(location = 2) in vec2	in_Texcoord;
uniform t_Matrix				Matrix;

out vec2	frag_Texcoord;
void main()
{
	frag_Texcoord = in_Texcoord;
	gl_Position = Matrix.ModelViewProjection * vec4(in_Position, 1);
}
