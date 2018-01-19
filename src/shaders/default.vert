#version 450 core

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
layout(location = 3) in vec3	in_Tangent;
layout(location = 4) in vec3	in_Bitangent;
uniform mat4					in_Transform;
uniform mat4					in_NormalMatrix;

out vec3			frag_Position;
out vec3			frag_Normal;
out vec2			frag_Texcoord;
out vec3			frag_Tangent;
out vec3			frag_Bitangent;
out mat3			frag_NormalMatrix;

void main()
{
	gl_Position = in_Transform * vec4(in_Position, 1);
	frag_Position = in_Position;
	frag_Normal = normalize(in_Normal * mat3(in_NormalMatrix));
	frag_Texcoord = in_Texcoord;
	frag_NormalMatrix = mat3(in_NormalMatrix);
	frag_Tangent = in_Tangent;
	frag_Bitangent = in_Bitangent;
	//gl_Position = in_Transform * in_View * in_Projection * vec4(in_Position, 1);
	//gl_Position = in_Projection * in_View * in_Transform * vec4(in_Position, 1);
	//gl_Position.w = 1.0;
}
