#version 450 core

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Depth;

in vec2				frag_UV;

out vec4			out_Color;

const int			kernel_size = 9;
const float			gaussian_kernel[] = {
	0.091637, 0.105358, 0.1164, 0.123573, 0.126061, 0.123573, 0.1164, 0.105358, 0.091637
};

void main()
{
	vec3	color = texture(in_Texture_Color, frag_UV).rgb;
	vec3	normal = texture(in_Texture_Normal, frag_UV).xyz;
	float	depth = texture(in_Texture_Depth, frag_UV).r;
	float	sampledist = 2.5f;
	vec3	finalColor = vec3(0);
	float	brightness = 0;
	vec2	texture_Size = 1.f / textureSize(in_Texture_Color, 0);
	float	occlusion = 0;
	for (int i = 0; i < kernel_size; i++) 
	{
		for (int j = 0; j < kernel_size; j++) 
		{
			vec2	offset = vec2(float(i - kernel_size / 2.f) * sampledist, float(j - kernel_size / 2.f) * sampledist) * texture_Size;
			vec3	sampleColor = texture(in_Texture_Color, frag_UV + offset).rgb;
			float	weight = gaussian_kernel[i] * gaussian_kernel[j];
			brightness += (max(0, sampleColor.r - 1) + max(0, sampleColor.g - 1) + max(0, sampleColor.z - 1) * weight);
			finalColor += (sampleColor * weight);
			offset = vec2(float(i - kernel_size / 2.f) * (1 + (1 - depth) * 2.5f), float(j - kernel_size / 2.f) * (1 + (1 - depth) * 2.5f)) * texture_Size;
			float	sampleDepth = texture(in_Texture_Depth, frag_UV + offset).r;
			if (depth == 1 || sampleDepth == 1)
				continue;
			vec3	sampleNormal = texture(in_Texture_Normal, frag_UV + offset).xyz;
			float	difference = max(0, depth - sampleDepth);
			float	angle = max(0, dot(sampleNormal, normal));
			occlusion += angle / (1.f + difference) * weight;
		}
	}
	occlusion = depth == 1 ? 0 : 1 - occlusion;
	occlusion *= 0.5f;
	brightness /=  float(kernel_size * kernel_size);
	finalColor *= brightness;
	finalColor += color - occlusion;
	//out_Color = vec4(texture(in_Texture_Color, frag_UV).xyz, 1);
	out_Color = vec4(finalColor, 1);
	//out_Color = vec4(vec3(occlusion), 1);
}
