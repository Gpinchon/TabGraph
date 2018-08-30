#version 430
#pragma optionNV (unroll all)
#define	KERNEL_SIZE 12

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

uniform vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(-.326,-.406),
	vec2(-.840,-.074),
	vec2(-.696, .457),
	vec2(-.203, .621),
	vec2( .962,-.195),
	vec2( .473,-.480),
	vec2( .519, .767),
	vec2( .185,-.893),
	vec2( .507, .064),
	vec2( .896, .412),
	vec2(-.322,-.933),
	vec2(-.792,-.598));

/* uniform vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(-0.613392, 0.617481),
	vec2(0.170019, -0.040254),
	vec2(-0.299417, 0.791925),
	vec2(0.645680, 0.493210),
	vec2(-0.651784, 0.717887),
	vec2(0.421003, 0.027070),
	vec2(-0.817194, -0.271096),
	vec2(-0.705374, -0.668203),
	vec2(0.977050, -0.108615),
	vec2(0.063326, 0.142369),
	vec2(0.203528, 0.214331),
	vec2(-0.667531, 0.326090),
	vec2(-0.098422, -0.295755),
	vec2(-0.885922, 0.215369),
	vec2(0.566637, 0.605213),
	vec2(0.039766, -0.396100),
	vec2(0.751946, 0.453352),
	vec2(0.078707, -0.715323),
	vec2(-0.075838, -0.529344),
	vec2(0.724479, -0.580798),
	vec2(0.222999, -0.215125),
	vec2(-0.467574, -0.405438),
	vec2(-0.248268, -0.814753),
	vec2(0.354411, -0.887570),
	vec2(0.175817, 0.382366),
	vec2(0.487472, -0.063082),
	vec2(-0.084078, 0.898312),
	vec2(0.488876, -0.783441),
	vec2(0.470016, 0.217933),
	vec2(-0.696890, -0.549791),
	vec2(-0.149693, 0.605762),
	vec2(0.034211, 0.979980),
	vec2(0.503098, -0.308878),
	vec2(-0.016205, -0.872921),
	vec2(0.385784, -0.393902),
	vec2(-0.146886, -0.859249),
	vec2(0.643361, 0.164098),
	vec2(0.634388, -0.049471),
	vec2(-0.688894, 0.007843),
	vec2(0.464034, -0.188818),
	vec2(-0.440840, 0.137486),
	vec2(0.364483, 0.511704),
	vec2(0.034028, 0.325968),
	vec2(0.099094, -0.308023),
	vec2(0.693960, -0.366253),
	vec2(0.678884, -0.204688),
	vec2(0.001801, 0.780328),
	vec2(0.145177, -0.898984),
	vec2(0.062655, -0.611866),
	vec2(0.315226, -0.604297),
	vec2(-0.780145, 0.486251),
	vec2(-0.371868, 0.882138),
	vec2(0.200476, 0.494430),
	vec2(-0.494552, -0.711051),
	vec2(0.612476, 0.705252),
	vec2(-0.578845, -0.768792),
	vec2(-0.772454, -0.090976),
	vec2(0.504440, 0.372295),
	vec2(0.155736, 0.065157),
	vec2(0.391522, 0.849605),
	vec2(-0.620106, -0.328104),
	vec2(0.789239, -0.419965),
	vec2(-0.545396, 0.538133),
	vec2(-0.178564, -0.596057)); */

layout(location = 0) out vec4	out_Albedo;
layout(location = 1) out vec4	out_Fresnel;
layout(location = 2) out vec4	out_Emitting;
layout(location = 3) out vec4	out_Material_Values; //Roughness, Metallic, AO
layout(location = 4) out vec4	out_BRDF;
layout(location = 5) out vec4	out_Normal;
layout(location = 6) out vec4	out_Position;

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;

in vec2	frag_UV;

void main()
{
	vec4	Albedo = texture(in_Texture_Albedo, frag_UV);
	vec4	Fresnel = texture(in_Texture_Fresnel, frag_UV);
	vec4	Emitting = texture(in_Texture_Emitting, frag_UV);
	vec4	Material_Values = texture(in_Texture_Material_Values, frag_UV);
	vec4	BRDF = texture(in_Texture_BRDF, frag_UV);
	vec4	Normal = texture(in_Texture_Normal, frag_UV);
	vec4	Position = texture(in_Texture_Position, frag_UV);
	gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;

	ivec2	tSize = textureSize(in_Texture_Position, 0);
	vec2	sampledist = tSize / 1024.f * 10.f;
	vec2	sampleOffset = sampledist / tSize;
	float	occlusion = 0.f;
	for (int i = 0; i < KERNEL_SIZE; ++i)
	{
		vec2	sampleUV = frag_UV + poissonDisk[i] * sampleOffset;
		vec3	samplePosition = texelFetch(in_Texture_Position, ivec2(tSize * sampleUV), 0).xyz;
		vec3	sampleNormal = texelFetch(in_Texture_Normal, ivec2(tSize * sampleUV), 0).xyz;
		vec3	V = samplePosition - Position.xyz;
		float	D = length(V);
		float	bias = D + 0.0025;
		float	factor = max(0, dot(Normal.xyz, normalize(V)));
		if (factor <= 0.5 && dot(sampleNormal, Normal.xyz) >= 0.8f)
			continue ;
		float	angle = max(0, factor - bias);
		occlusion += (angle * (1.f / (1.f + D)));
	}
	occlusion /= float(KERNEL_SIZE);
	Material_Values.z += occlusion;
	Material_Values.z = clamp(Material_Values.z, 0, 1);
	out_Albedo = Albedo;
	out_Fresnel = Fresnel;
	out_Emitting = Emitting;
	out_Material_Values = Material_Values;
	out_BRDF = BRDF;
	out_Normal = Normal;
	out_Position = Position;
}