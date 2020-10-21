R""(

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

uniform t_MetallicRoughnessValues	_MetallicRoughnessValues;
#ifdef METALLIC_ROUGHNESS_USE_TEXTURES
uniform t_MetallicRoughnessTextures	MetallicRoughnessTextures;
#endif

t_MetallicRoughnessValues MetallicRoughnessValues;

void MetallicRoughness() {
	MetallicRoughnessValues = _MetallicRoughnessValues;
	MetallicRoughnessValues.BaseColor = _MetallicRoughnessValues.BaseColor;
	MetallicRoughnessValues.Roughness = _MetallicRoughnessValues.Roughness;
	MetallicRoughnessValues.Metallic = _MetallicRoughnessValues.Metallic;
	MetallicRoughnessValues.BaseColor.rgb *= CDiff();
	MetallicRoughnessValues.BaseColor.a *= Opacity();
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR
	MetallicRoughnessValues.BaseColor *= texture(MetallicRoughnessTextures.BaseColor, TexCoord());
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS
	MetallicRoughnessValues.Roughness *= texture(MetallicRoughnessTextures.Roughness, TexCoord()).r;
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC
	MetallicRoughnessValues.Metallic *= texture(MetallicRoughnessTextures.Metallic, TexCoord()).r;
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS
	vec2	metallicRoughness_sample = texture(MetallicRoughnessTextures.MetallicRoughness, TexCoord()).gb;
	MetallicRoughnessValues.Roughness *= metallicRoughness_sample.x;
	MetallicRoughnessValues.Metallic *= metallicRoughness_sample.y;
#endif
	SetCDiff(mix(MetallicRoughnessValues.BaseColor.rgb * (1 - F0()), vec3(0), MetallicRoughnessValues.Metallic));
	SetF0(mix(F0(), MetallicRoughnessValues.BaseColor.rgb, MetallicRoughnessValues.Metallic));
	SetAlpha(Alpha() * map(MetallicRoughnessValues.Roughness * MetallicRoughnessValues.Roughness, 0, 1, 0.05, 1));
	//MetallicRoughnessValues.Roughness = map(MetallicRoughnessValues.Roughness, 0, 1, 0.05, 1);
	//Frag.Material.Specular = mix(Frag.Material.Specular, Frag.Material.Albedo.rgb, MetallicRoughnessValues.Metallic);
}

)""