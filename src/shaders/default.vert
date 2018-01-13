#version 330 core

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec3	in_Tangent;
layout(location = 3) in vec2	in_Texcoord;
uniform mat4					in_Transform;
uniform mat4					in_NormalMatrix;

out vec3			frag_Position;
out vec3			frag_Normal;
out vec2			frag_Texcoord;

void main()
{
	gl_Position = in_Transform * vec4(in_Position, 1);
	frag_Position = in_Position;
	frag_Normal = in_Normal * mat3(in_NormalMatrix);
	frag_Texcoord = in_Texcoord;
	//gl_Position = in_Transform * in_View * in_Projection * vec4(in_Position, 1);
	//gl_Position = in_Projection * in_View * in_Transform * vec4(in_Position, 1);
	//gl_Position.w = 1.0;
}
