#version 410
uniform sampler2D	in_Texture_Albedo;
uniform bool		in_Use_Texture_Albedo;
in vec2				frag_Texcoord;

void main()
{
	if (in_Use_Texture_Albedo)
		if (texture(in_Texture_Albedo, frag_Texcoord).a <= 0.5f)
			discard;
}
