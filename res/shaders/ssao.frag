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
layout(location = 6) out vec4	out_Position;

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_Ior;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_AO;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;

in vec2	frag_UV;

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
	out_Normal = texture(in_Texture_Normal, frag_UV);
	out_Position = texture(in_Texture_Position, frag_UV);
	out_Albedo = texture(in_Texture_Albedo, frag_UV);
	out_Fresnel = texture(in_Texture_Fresnel, frag_UV);
	out_Emitting = texture(in_Texture_Emitting, frag_UV);
	out_Material_Values = texture(in_Texture_Material_Values, frag_UV);
	out_AO = texture(in_Texture_AO, frag_UV);
	const float	Depth = gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;

	if (Depth == 1)
		return ;

	float	sampleOffset = 0.5f * (1 - Depth);
	float	sampleAngle = randomAngle(out_Position.xyz, 1024);
	float	s = sin(sampleAngle);
	float	c = cos(sampleAngle);
	vec2	sampleRotation = vec2(c, -s);
	float	occlusion = 0.f;
	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		vec2	sampleUV = frag_UV + poissonDisk[i] * sampleRotation * sampleOffset;
		vec3	samplePosition = texture(in_Texture_Position, sampleUV).xyz;
		vec3	V = samplePosition - out_Position.xyz;
		float	D = length(V);
		float	bias = D + 0.025;
		float	factor = max(0, dot(out_Normal.xyz, normalize(V)));
		float	angle = max(0, factor - bias);
		occlusion += (angle * (1.f / (1.f + D)));
	}
	occlusion /= float(KERNEL_SIZE);
	out_AO.r += max(0, occlusion);
}