R""(

#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS
#undef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR
#undef SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS
#endif

#if defined(SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE) || defined(SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS) || defined(SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR) || defined(SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS)
#define SPECULAR_GLOSSINESS_USE_TEXTURES
#endif
//#ifdef TEXTURE_USE_SPECULAR
//uniform sampler2D	TextureSpecular;
//#endif
#ifdef SPECULAR_GLOSSINESS_USE_TEXTURES
struct t_SpecularGlossinessTextures {
	#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE
	sampler2D	Diffuse;
	#endif
	#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS
	sampler2D	SpecularGlossiness;
	#endif
};
#endif

struct t_SpecularGlossinessValues {
	vec4		Diffuse;
	vec3		Specular;
	float		Glossiness;
};

uniform t_SpecularGlossinessValues	SpecularGlossinessValues;
#ifdef SPECULAR_GLOSSINESS_USE_TEXTURES
uniform t_SpecularGlossinessTextures	SpecularGlossinessTextures;
#endif


void SpecularGlossiness() {
	t_SpecularGlossinessValues values;
	values.Diffuse = SpecularGlossinessValues.Diffuse;
	values.Specular = SpecularGlossinessValues.Specular;
	values.Glossiness = SpecularGlossinessValues.Glossiness;
#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE
	values.Diffuse *= texture(SpecularGlossinessTextures.Diffuse, TexCoord());
#endif
#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR
	values.Specular *= texture(SpecularGlossinessTextures.Specular, TexCoord()).rgb;
#endif
#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS
	vec4	SpecularGlossiness_sample = texture(SpecularGlossinessTextures.SpecularGlossiness, TexCoord());
	values.Specular *= SpecularGlossiness_sample.xyz;
	values.Glossiness *= SpecularGlossiness_sample.w;
#endif
	SetCDiff(CDiff() * values.Diffuse.rgb * (1 - max(values.Specular.r, max(values.Specular.g, values.Specular.b))));
	SetF0(values.Specular);
	SetAlpha(pow(1 - values.Glossiness, 2));
	SetOpacity(Opacity() * values.Diffuse.a);
	//SetCDiff(mix(values.Diffuse.rgb * (1 - F0()), vec3(0), values.Glossiness));
	//SetF0(mix(F0(), values.Diffuse.rgb, values.Glossiness));
	//SetAlpha(Alpha() * map(values.Roughness * values.Roughness, 0, 1, 0.05, 1));
}

)""