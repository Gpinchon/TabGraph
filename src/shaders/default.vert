#version 410

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
uniform mat4					in_Transform;
uniform mat4					in_ModelMatrix;
uniform mat4					in_NormalMatrix;
uniform mat4					in_ShadowTransform;

out vec3			frag_ShadowPosition;
out vec3			frag_WorldPosition;
out vec3			frag_WorldNormal;
out vec3			frag_ModelPosition;
out vec3			frag_ModelNormal;
out vec2			frag_Texcoord;

mat4 biasMatrix = mat4( 
0.5, 0.0, 0.0, 0.0, 
0.0, 0.5, 0.0, 0.0, 
0.0, 0.0, 0.5, 0.0, 
0.5, 0.5, 0.5, 1.0 
);

void main()
{
	frag_ModelPosition = in_Position;
	frag_ModelNormal = (in_Normal / 255.f) * 2 - 1;
	gl_Position = in_Transform * vec4(in_Position, 1);
	frag_WorldPosition = vec3(in_ModelMatrix * vec4(in_Position, 1));
	frag_ShadowPosition = vec3((biasMatrix * in_ShadowTransform) * vec4(in_Position, 1));
	frag_WorldNormal = normalize(mat3(in_NormalMatrix) * frag_ModelNormal);
	frag_Texcoord = in_Texcoord;
}
