#version 430

out vec4			out_Color;

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Depth;

in vec2	frag_UV;

void main()
{
	out_Color.rgb = texture(in_Texture_Color, frag_UV).rgb + texture(in_Texture_Emitting, frag_UV).rgb;
	out_Color.a = 1;
	gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
}