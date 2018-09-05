#version 430
#pragma optionNV (unroll all)

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

layout(location = 0) out vec4	out_Albedo;
layout(location = 1) out vec4	out_Emitting;
layout(location = 2) out vec4	out_Fresnel;
layout(location = 3) out vec4	out_Material_Values; //Roughness, Metallic, Ior
layout(location = 4) out vec4	out_AO;
layout(location = 5) out vec4	out_Normal;

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_Ior;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_AO;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Depth;

uniform mat4		in_InvProjViewMatrix;

in vec2	frag_UV;

struct t_Material {
	vec4		Albedo;
	vec3		Specular;
	vec3		Emitting;
	float		Roughness;
	float		Metallic;
	float		Ior;
	float		AO;
};

struct t_Frag {
	float		Depth;
	vec2		UV;
	vec3		Position;
	vec3		Normal;
	t_Material	Material;
};

t_Frag	Frag;

vec3	WorldPosition(vec2 UV)
{
	float	linearDepth = texture(in_Texture_Depth, UV).r * 2.0 - 1.0;
	vec2	coord = UV * 2.0 - 1.0;
	vec4	projectedCoord = vec4(coord, linearDepth, 1.0);
	projectedCoord = in_InvProjViewMatrix * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

vec3	WorldPosition()
{
	float	linearDepth = Frag.Depth * 2.0 - 1.0;
	vec2	coord = frag_UV * 2.0 - 1.0;
	vec4	projectedCoord = vec4(coord, linearDepth, 1.0);
	projectedCoord = in_InvProjViewMatrix * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

void	FillFrag()
{
	Frag.UV = frag_UV;
	Frag.Depth = gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
	Frag.Position = WorldPosition();
	Frag.Normal = texture(in_Texture_Normal, frag_UV).xyz;
	Frag.Material.Albedo = texture(in_Texture_Albedo, frag_UV);
	Frag.Material.Specular = texture(in_Texture_Fresnel, frag_UV).xyz;
	Frag.Material.Emitting = texture(in_Texture_Emitting, frag_UV).xyz;
	vec3	Material_Values = texture(in_Texture_Material_Values, frag_UV).xyz;
	Frag.Material.Roughness = Material_Values.x;
	Frag.Material.Metallic = Material_Values.y;
	Frag.Material.Ior = Material_Values.z;
	Frag.Material.AO = texture(in_Texture_AO, frag_UV).r;
}

void	FillOut()
{
	gl_FragDepth = Frag.Depth;
	out_Albedo = Frag.Material.Albedo;
	out_Fresnel = vec4(Frag.Material.Specular, 1);
	out_Emitting = vec4(Frag.Material.Emitting, 1);
	out_Material_Values = vec4(Frag.Material.Roughness, Frag.Material.Metallic, Frag.Material.Ior, 1);
	out_AO = vec4(Frag.Material.AO, 0, 0, 1);
	out_Normal = vec4(Frag.Normal, 1);
}

float	random(in vec3 seed, in float freq)
{
   float dt = dot(floor(seed * freq), vec3(53.1215, 21.1352, 9.1322));
   return fract(sin(dt) * 2105.2354);
}

float	randomAngle(in vec3 seed, in float freq)
{
   return random(seed, freq) * 6.283285;
}

void	ApplyTechnique();

void main()
{
	FillFrag();
	ApplyTechnique();
	FillOut();
}

