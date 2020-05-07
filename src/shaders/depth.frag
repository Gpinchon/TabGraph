R""(
struct t_Textures {
	vec2		Scale;
#ifdef TEXTURE_USE_ALBEDO
	sampler2D	Albedo;
#endif
};

struct t_Material {
	float		Alpha;
};

uniform t_Material		Material;
uniform t_Textures		Texture;

in vec2				frag_Texcoord;

void main()
{
	float alpha = Material.Alpha;
#ifdef TEXTURE_USE_ALBEDO
	alpha *= texture(Texture.Albedo, frag_Texcoord).a;
#endif
	if (alpha <= 0.05f)
		discard;
}

)""