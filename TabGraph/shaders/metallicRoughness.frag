R""(

#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS
#undef METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS
#undef METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC
#endif

#if defined(METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR) || defined(METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS) || defined(METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC) || defined(METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS)
#define METALLIC_ROUGHNESS_USE_TEXTURES
#endif

//#ifdef TEXTURE_USE_SPECULAR
//uniform sampler2D	TextureSpecular;
//#endif
#ifdef METALLIC_ROUGHNESS_USE_TEXTURES
struct t_MetallicRoughnessTextures {
	#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR
	sampler2D	BaseColor;
	#endif
	#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS
	sampler2D	Roughness;
	#endif
	#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC
	sampler2D	Metallic;
	#endif
	#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS
	sampler2D	MetallicRoughness;
	#endif
};
#endif

struct t_MetallicRoughnessValues {
	vec4		BaseColor;
	float		Roughness;
	float		Metallic;
};

uniform t_MetallicRoughnessValues	MetallicRoughnessValues;
#ifdef METALLIC_ROUGHNESS_USE_TEXTURES
uniform t_MetallicRoughnessTextures	MetallicRoughnessTextures;
#endif


void MetallicRoughness() {
	t_MetallicRoughnessValues values;
	values.BaseColor = MetallicRoughnessValues.BaseColor;
	values.Roughness = MetallicRoughnessValues.Roughness;
	values.Metallic = MetallicRoughnessValues.Metallic;
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR
	values.BaseColor *= texture(MetallicRoughnessTextures.BaseColor, TexCoord());
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS
	values.Roughness *= texture(MetallicRoughnessTextures.Roughness, TexCoord()).r;
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC
	values.Metallic *= texture(MetallicRoughnessTextures.Metallic, TexCoord()).r;
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS
	vec2	metallicRoughness_sample = texture(MetallicRoughnessTextures.MetallicRoughness, TexCoord()).gb;
	values.Roughness *= metallicRoughness_sample.x;
	values.Metallic *= metallicRoughness_sample.y;
#endif
	SetCDiff(CDiff() * mix(values.BaseColor.rgb * (1 - F0()), vec3(0), values.Metallic));
	SetF0(mix(F0(), values.BaseColor.rgb, values.Metallic));
	SetAlpha(Alpha() * map(values.Roughness * values.Roughness, 0, 1, 0.05, 1));
	SetOpacity(Opacity() * values.BaseColor.a);
}

)""