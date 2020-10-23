R""(
uniform sampler2D	opaqueBackColor;
uniform sampler2D	opaqueBackEmissive;
uniform sampler2D	opaqueBackNormal;
uniform sampler2D	opaqueBackDepth;

float	cmix(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (mix(min, max, 1 - mod(percent, 1)));
	return (mix(min, max, percent));
}

vec2	cmix(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(cmix(min.x, max.x, percent.x), cmix(min.y, max.y, percent.y)));
}

float	warpUV(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (smoothstep(min, max, 1 - mod(percent, 1)));
	return (percent);
}

vec2	warpUV(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(warpUV(min.x, max.x, percent.x), warpUV(min.y, max.y, percent.y)));
}

vec3	Fresnel(in float cosT, in vec3 f0)
{
  return (f0 + (1 - f0) * pow(1 - cosT, 5));
}

vec3	RefractionF0(in float ior)
{
	float	f0 = abs((1.0 - ior) / (1.0 + ior));
	return (vec3(f0 * f0));
}

void	Refraction()
{
	vec2	refract_UV = TexCoord();

	if (Ior() != 1)
	{
		vec3	V = normalize(Camera.Position - WorldPosition());
		float	NdV = dot(WorldNormal(), V);
		vec3	fresnel = Fresnel(NdV, RefractionF0(Ior()));
		//vec3	fresnel = Fresnel(NdV, F0());
		vec2	refractFactor = vec2((1 - Depth()) * max(fresnel.x, max(fresnel.y, fresnel.z)));
		vec2	refractDir = (mat3(Camera.Matrix.View) * normalize(refract(V, WorldNormal(), 1.0 / Ior()))).xy;
		refract_UV = refractDir * refractFactor + TexCoord();
		refract_UV = warpUV(vec2(0), vec2(1), refract_UV);
	}
	vec3	Back_Color = sampleLod(Texture.Back.Color, refract_UV, Alpha()).rgb;
	vec3	Back_Emissive = sampleLod(Texture.Back.Emissive, refract_UV, Alpha()).rgb;
	vec3	Back_Normal = texture(opaqueBackNormal, refract_UV).rgb;
	if (Opacity() == 0) {
		return ;
	}
	vec3 refractionColor = mix(vec3(1), CDiff(), Opacity());
	vec3 backColor = mix(Back_Color * refractionColor, vec3(0), Opacity());
	vec3 backEmissive = mix(Back_Emissive * refractionColor, vec3(0), Opacity());
	SetBackColor(vec4(backColor, 1));
	SetBackEmissive(backEmissive);
}


)""