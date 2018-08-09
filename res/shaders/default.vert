#version 410

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
uniform mat4					in_Transform;
uniform mat4					in_ModelMatrix;
uniform mat4					in_NormalMatrix;
uniform samplerCube				in_Texture_Env_Spec;
uniform lowp vec2				in_UVMax;
uniform lowp vec2				in_UVMin;
uniform lowp vec2				in_UVScale;

out vec3						frag_WorldPosition;
out lowp vec3					frag_WorldNormal;
out lowp vec2					frag_Texcoord;
out lowp vec2					frag_UVMax;
out lowp vec2					frag_UVMin;

mat4 biasMatrix = mat4( 
0.5, 0.0, 0.0, 0.0, 
0.0, 0.5, 0.0, 0.0, 
0.0, 0.0, 0.5, 0.0, 
0.5, 0.5, 0.5, 1.0 
);

void main()
{
	frag_WorldPosition = vec3(in_ModelMatrix * vec4(in_Position, 1));
	frag_WorldNormal = mat3(in_NormalMatrix) * ((in_Normal / 255.f) * 2 - 1);
	frag_Texcoord = in_Texcoord * in_UVScale;
	frag_UVMax = in_UVMax * in_UVScale;
	frag_UVMin = in_UVMin * in_UVScale;
	gl_Position = in_Transform * vec4(in_Position, 1);
}
