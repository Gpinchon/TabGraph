R""(

#if defined(TEXTURE_USE_ROUGHNESS) || defined(TEXTURE_USE_METALLIC) || defined(TEXTURE_USE_METALLICROUGHNESS)
#define USE_TEXTURES_METALLIC_ROUGHNESS
#endif
//#ifdef TEXTURE_USE_SPECULAR
//uniform sampler2D	TextureSpecular;
//#endif
#ifdef USE_TEXTURES_METALLIC_ROUGHNESS
struct t_MetallicRoughnessTextures {
	#ifdef TEXTURE_USE_ROUGHNESS
	sampler2D	Roughness;
	#endif
	#ifdef TEXTURE_USE_METALLIC
	sampler2D	Metallic;
	#endif
	#ifdef TEXTURE_USE_METALLICROUGHNESS
	sampler2D	MetallicRoughness;
	#endif
};
#endif

struct t_MetallicRoughnessValues {
	float		Roughness;
	float		Metallic;
};

struct t_MetallicRoughness {
	t_MetallicRoughnessValues	Value;
#ifdef USE_TEXTURES_METALLIC_ROUGHNESS
	t_MetallicRoughnessTextures	Texture;
#endif
};

uniform t_MetallicRoughnessValues	_MetallicRoughnessValues;
#ifdef USE_TEXTURES_METALLIC_ROUGHNESS
uniform t_MetallicRoughnessTextures	MetallicRoughnessTextures;
#endif

t_MetallicRoughnessValues MetallicRoughnessValues;

void FillMetallicRoughness() {


//#ifdef TEXTURE_USE_SPECULAR
//	Frag.Material.Specular = texture(Texture.Specular, Frag.UV).rgb;
//#endif
	MetallicRoughnessValues = _MetallicRoughnessValues;
#ifdef TEXTURE_USE_ROUGHNESS
	MetallicRoughnessValues.Roughness *= texture(MetallicRoughnessTextures.Roughness, Frag.UV).r;
#endif
#ifdef TEXTURE_USE_METALLIC
	MetallicRoughnessValues.Metallic *= texture(MetallicRoughnessTextures.Metallic, Frag.UV).r;
#endif
#ifdef TEXTURE_USE_METALLICROUGHNESS
	vec2	metallicRoughness_sample = texture(MetallicRoughnessTextures.MetallicRoughness, Frag.UV).gb;
	MetallicRoughnessValues.Roughness *= metallicRoughness_sample.x;
	MetallicRoughnessValues.Metallic *= metallicRoughness_sample.y;
#endif
	vec3 viewDir = normalize(Camera.Position - Frag.Position);
	if (dot(viewDir, Frag.Normal) < 0)
		Frag.Normal = -Frag.Normal;
	MetallicRoughnessValues.Roughness = map(MetallicRoughnessValues.Roughness, 0, 1, 0.05, 1);
	//Frag.Material.Specular = mix(Frag.Material.Specular, Frag.Material.Albedo.rgb, MetallicRoughnessValues.Metallic);
}

void	ApplyTechnique()
{
	FillMetallicRoughness();
	if (StandardValues.Opacity <= 0.05f)
		discard;
	const vec3 dielectricSpecular = vec3(0.04);
	Frag.BRDF.CDiff = mix(StandardValues.Diffuse.rgb * (1 - dielectricSpecular), vec3(0), MetallicRoughnessValues.Metallic);
	Frag.BRDF.F0 = mix(dielectricSpecular, StandardValues.Diffuse.rgb, MetallicRoughnessValues.Metallic);
	Frag.BRDF.Alpha = MetallicRoughnessValues.Roughness * MetallicRoughnessValues.Roughness;
}

)""