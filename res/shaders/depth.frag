struct t_Textures {
	sampler2D	Albedo;
	bool		Use_Albedo;
};

struct t_Material {
	t_Textures	Texture;
};

uniform t_Material		Material;

in vec2				frag_Texcoord;

void main()
{
	float a = texture(Material.Texture.Albedo, frag_Texcoord).a;
	if (Material.Texture.Use_Albedo && a <= 0.05f)
		discard;
}
