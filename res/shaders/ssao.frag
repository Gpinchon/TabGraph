#version 430
#pragma optionNV (unroll all)
#define	KERNEL_SIZE 9

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

uniform vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(0.95581, -0.18159), vec2(0.50147, -0.35807), vec2(0.69607, 0.35559),
	vec2(-0.0036825, -0.59150),	vec2(0.15930, 0.089750), vec2(-0.65031, 0.058189),
	vec2(0.11915, 0.78449),	vec2(-0.34296, 0.51575), vec2(-0.60380, -0.41527));

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

float random(in vec3 seed, in float freq)
{
   float dt = dot(floor(seed * freq), vec3(53.1215, 21.1352, 9.1322));
   return fract(sin(dt) * 2105.2354);
}

float randomAngle(in vec3 seed, in float freq)
{
   return random(seed, freq) * 6.283285;
}

void main()
{
	FillFrag();

	if (Frag.Depth == 1)
		return ;

	float	sampleOffset = 0.5f * (1 - Frag.Depth);
	float	sampleAngle = randomAngle(Frag.Position, 1024);
	float	s = sin(sampleAngle);
	float	c = cos(sampleAngle);
	vec2	sampleRotation = vec2(c, -s);
	float	occlusion = 0.f;
	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		vec2	sampleUV = frag_UV + poissonDisk[i] * sampleRotation * sampleOffset;
		vec3	samplePosition = WorldPosition(sampleUV);
		vec3	V = samplePosition - Frag.Position.xyz;
		float	D = length(V);
		float	bias = D + 0.025;
		float	factor = max(0, dot(Frag.Normal, normalize(V)));
		float	angle = max(0, factor - bias);
		occlusion += (angle * (1.f / (1.f + D)));
	}
	occlusion /= float(KERNEL_SIZE);
	Frag.Material.AO += max(0, occlusion);
	FillOut();
}