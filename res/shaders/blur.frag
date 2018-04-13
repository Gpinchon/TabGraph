#version 410
#define	KERNEL_SIZE 9

uniform sampler2D	in_Texture_Color;
uniform vec2		in_Direction;

in vec2				frag_UV;

out vec4			out_Color;

uniform float		gaussian_kernel[] = float[KERNEL_SIZE](
	0.033619, 0.072215, 0.124675, 0.173006, 0.192969, 0.173006, 0.124675, 0.072215, 0.033619
);

void main()
{
	out_Color = vec4(0);
	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		float	weight = gaussian_kernel[i];
		vec2	index = vec2(float(i - (KERNEL_SIZE - 1) / 2.f), float(i - (KERNEL_SIZE - 1) / 2.f)) * in_Direction;
		vec2	sampleUV = frag_UV + (index / textureSize(in_Texture_Color, 0));
		vec4	color = texture(in_Texture_Color, sampleUV);
		out_Color += color * weight;
	}
}
