#version 410

struct t_Material {
	vec3		Albedo;
	vec3		Specular;
	vec3		Emitting;
	float		Roughness;
	float		Metallic;
	float		Alpha;
	float		Parallax;
	t_Textures	Texture;
};


layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec2	in_Texcoord;
uniform mat4					in_Transform;
uniform mat4					in_ModelMatrix;
uniform mat4					in_NormalMatrix;
uniform samplerCube				Environment.Irradiance;
uniform lowp vec2				Material.UVScale;

out vec3						frag_WorldPosition;
out lowp vec3					frag_WorldNormal;
out lowp vec2					frag_Texcoord;

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
	frag_Texcoord = Material.UVScale * in_Texcoord;
	gl_Position = in_Transform * vec4(in_Position, 1);
}
