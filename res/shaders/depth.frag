#version 430
uniform sampler2D	in_Texture_Albedo;
uniform bool		in_Use_Texture_Albedo;
in vec2				frag_Texcoord;

void main()
{
	float a = texture(in_Texture_Albedo, frag_Texcoord).a;
	if (in_Use_Texture_Albedo && a <= 0.05f)
		discard;
}
