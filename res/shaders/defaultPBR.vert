#version 410

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
uniform vec3					in_LightDirection = normalize(vec3(-1, 1, 0));
uniform mat4					in_Transform;
uniform mat4					in_ModelMatrix;
uniform mat4					in_NormalMatrix;
uniform mat4					in_ShadowTransform;
uniform samplerCube				in_Texture_Env_Spec;

out vec3						frag_ShadowPosition;
out vec3						frag_WorldPosition;
out lowp vec3					frag_WorldNormal;
out lowp vec2					frag_Texcoord;
out lowp vec3					frag_Light_Color;

mat4 biasMatrix = mat4( 
0.5, 0.0, 0.0, 0.0, 
0.0, 0.5, 0.0, 0.0, 
0.0, 0.0, 0.5, 0.0, 
0.5, 0.5, 0.5, 1.0 
);

void main()
{
	frag_WorldPosition = vec3(in_ModelMatrix * vec4(in_Position, 1));
	frag_ShadowPosition = vec3((biasMatrix * in_ShadowTransform) * vec4(in_Position, 1));
	frag_WorldNormal = mat3(in_NormalMatrix) * ((in_Normal / 255.f) * 2 - 1);
	frag_Texcoord = in_Texcoord;
	frag_Light_Color = vec3(0.5) * texture(in_Texture_Env_Spec, -in_LightDirection).rgb + 0.5;
	gl_Position = in_Transform * vec4(in_Position, 1);
}
