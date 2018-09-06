struct t_Textures {
	sampler2D	Albedo;
	bool		Use_Albedo;
};

struct t_Material {
	float		Alpha;
};

uniform t_Material		Material;
uniform t_Textures		Texture;

in vec2				frag_Texcoord;

void main()
{
	float a = texture(Texture.Albedo, frag_Texcoord).a;
	float alpha = Material.Alpha;
	if (Texture.Use_Albedo)
		alpha *= a;
	if (alpha <= 0.05f)
		discard;
}
