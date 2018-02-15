#version 410 core
uniform sampler2D	in_Texture_Albedo;
uniform bool		in_Use_Texture_Albedo;
in vec2				frag_Texcoord;

void main()
{
	if (in_Use_Texture_Albedo)
		if (texture(in_Texture_Albedo, frag_Texcoord).a <= 0.05f)
			discard;
}
