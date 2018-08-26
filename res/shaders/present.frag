#version 410
out vec4			out_Color;

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Emitting;

in vec2	frag_UV;

void main()
{
	out_Color.rgb = texture(in_Texture_Color, frag_UV).rgb + texture(in_Texture_Emitting, frag_UV).rgb;
	out_Color.a = 1;
}