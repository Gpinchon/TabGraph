#version 330 core

layout(location = 0) in vec3	in_Vertex;
uniform mat4					in_Transform;

void main()
{
	gl_Position = in_Transform * vec4(in_Vertex, 1);
	//gl_Position = in_Transform * in_View * in_Projection * vec4(in_Vertex, 1);
	//gl_Position = in_Projection * in_View * in_Transform * vec4(in_Vertex, 1);
	//gl_Position.w = 1.0;
}
