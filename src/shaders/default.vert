#version 450 core

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
uniform mat4					in_Transform;
uniform mat4					in_ModelMatrix;
uniform mat4					in_NormalMatrix;

out vec3			frag_WorldPosition;
out vec3			frag_WorldNormal;
out vec2			frag_Texcoord;

void main()
{
	gl_Position = in_Transform * vec4(in_Position, 1);
	frag_WorldPosition = vec3(in_ModelMatrix * vec4(in_Position, 1));
	frag_WorldNormal = normalize(mat3(in_NormalMatrix) * in_Normal);
	frag_Texcoord = in_Texcoord;
}
