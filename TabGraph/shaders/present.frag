R""(
out vec4			out_Color;

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Emissive;
uniform sampler2D	in_Texture_Depth;

in vec2	frag_UV;

void Present()
{
	out_Color.rgb = texture(in_Texture_Color, frag_UV).rgb + texture(in_Texture_Emissive, frag_UV).rgb;
	out_Color.a = 1;
	gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
}

)""