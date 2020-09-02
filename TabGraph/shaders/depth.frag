R""(
struct t_StandardValues {
	float		Opacity;
};

#ifdef TEXTURE_USE_DIFFUSE
struct t_Textures {
	sampler2D	Diffuse;
};
#endif

struct t_Material {
	float		Alpha;
};

#ifdef USE_TEXTURES
uniform t_Textures			StandardTextures;
#endif
uniform t_StandardValues	_StandardValues;
uniform t_Material			Material;

in vec2						frag_Texcoord;

void main()
{
	float alpha = _StandardValues.Opacity;
#ifdef TEXTURE_USE_DIFFUSE
	alpha *= texture(StandardTextures.Diffuse, frag_Texcoord).a;
#endif
	if (alpha <= 0.05f)
		discard;
}

)""