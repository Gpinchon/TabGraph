#version 330 core

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec3	in_Tangent;
layout(location = 3) in vec2	in_Texcoord;
uniform mat4					in_Transform;

void main()
{
	gl_Position = in_Transform * vec4(in_Position, 1);
	//gl_PointSize = distance(vec3(4, 3, 3), gl_Position.xyz);
	gl_PointSize = (1 - abs(1/gl_Position.z)) * 10;
	//gl_Position = in_Transform * in_View * in_Projection * vec4(in_Position, 1);
	//gl_Position = in_Projection * in_View * in_Transform * vec4(in_Position, 1);
	//gl_Position.w = 1.0;
}
