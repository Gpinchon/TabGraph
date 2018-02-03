#version 450 core
#define	KERNEL_SIZE 9

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;

in vec2				frag_UV;

out vec4			out_Color;

const float			gaussian_kernel[KERNEL_SIZE] = {
	0.091637, 0.105358, 0.1164, 0.123573, 0.126061, 0.123573, 0.1164, 0.105358, 0.091637
};

const vec2			poisson[64] = vec2[](
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
	vec2(-0.178564, -0.596057)
);

float	do_occlusion()
{
	float	occlusion = 0;
	vec3	position = texture(in_Texture_Position, frag_UV).xyz;
	vec3	normal = texture(in_Texture_Normal, frag_UV).xyz;
	vec3	sample_pos, sample_pos_diff;
	vec2	sample_UV;
	float	radius_factor = 0.01 + (1.0 / texture(in_Texture_Depth, frag_UV).r) * 0.04;
	vec2	radius = vec2(radius_factor, radius_factor);

	for (int i = 0; i < 64; ++i)
	{
		sample_UV = poisson[i] * radius + frag_UV;
		sample_pos = texture(in_Texture_Position, sample_UV).xyz;
		sample_pos_diff = sample_pos - position;
		//if (texture(diffuse_texture, sample_UV).w != 0)
		occlusion = dot(normal, normalize(sample_pos_diff)) * (2.0 / (1.0 + length(sample_pos_diff))) + occlusion;
	}
	return min(occlusion / float(64), 1);
}

vec3	texture_gaussian(sampler2D tex, vec2 tex_Coord, float width)
{
	vec2	texture_Size = textureSize(tex, 0);
	vec3	color = vec3(0);
	for (int x = 0; x < KERNEL_SIZE; x++)
	{
		for (int y = 0; y < KERNEL_SIZE; y++)
		{
			vec2	offset = vec2(x - KERNEL_SIZE / 2.f, y - KERNEL_SIZE / 2.f) / texture_Size * width;
			vec2	sampleUV = tex_Coord + offset;
			float	weight = gaussian_kernel[x] * gaussian_kernel[y];
			color += texture(tex, sampleUV).xyz * weight;
		}
	}
	return (color);
}

void main()
{
	out_Color = vec4(texture_gaussian(in_Texture_Color, frag_UV, 2.5f), 1);
	//out_Color = vec4(textureLod(in_Texture_Color, frag_UV, 5.f).xyz, 1);
	vec3	color = texture(in_Texture_Color, frag_UV).rgb;
	vec3	normal = texture(in_Texture_Normal, frag_UV).xyz;
	vec3	position = texture(in_Texture_Position, frag_UV).xyz;
	float	depth = texture(in_Texture_Depth, frag_UV).r;
	float	sampledist = 3.f;
	vec3	finalColor = vec3(0);
	float	brightness = 0.f;
	vec2	texture_Size = 1.f / textureSize(in_Texture_Color, 0);
	float	occlusion = 0.f;
	for (int i = 0; i < KERNEL_SIZE; i++) 
	{
		for (int j = 0; j < KERNEL_SIZE; j++) 
		{
			vec2	index = vec2(float(i - KERNEL_SIZE / 2.f), float(j - KERNEL_SIZE / 2.f));
			vec2	sampleUV = frag_UV + index * sampledist * texture_Size;
			vec3	sampleColor = texture(in_Texture_Color, sampleUV).rgb;
			float	weight = gaussian_kernel[i] * gaussian_kernel[j];
			brightness += (max(0, sampleColor.r - 1) + max(0, sampleColor.g - 1) + max(0, sampleColor.z - 1) * weight);
			finalColor += (sampleColor * weight);
			float	sampleDepth = texture(in_Texture_Depth, sampleUV).r;
			if (depth == 1 || sampleDepth == 1)
				continue;
			vec3	samplePosition = texture(in_Texture_Position, sampleUV).xyz;
			vec3	V = position - samplePosition;
			float	D = length(V);
			if (D < 0.001f)
				continue;
			D /= sampledist;
			float	bias = 0.001 + sampleDepth * 0.5f;
			float	factor = dot(normal, normalize(V));
			if (isnan(factor))
				continue;
			float	angle = max(0, factor - bias);
			occlusion += (angle * (1.f / (1.f + D))) * weight;
		}
	}
	occlusion = depth == 1 ? 0 : occlusion;
	brightness /=  float(KERNEL_SIZE * KERNEL_SIZE);
	finalColor *= brightness;
	finalColor += color - occlusion;
	out_Color = vec4(finalColor, 1);
}
