R""(
uniform sampler2D	opaqueBackColor;
uniform sampler2D	opaqueBackEmitting;
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

vec3	Fresnel(in float cosT, in vec3 F0)
{
  return (F0 + (1 - F0) * pow(1 - cosT, 5));
}

vec3	F0(in float ior)
{
	float	f0 = abs((1.0 - ior) / (1.0 + ior));
	return (vec3(f0 * f0));
}

void	ApplyTechnique()
{
	vec2	refract_UV = Frag.UV;

	if (Frag.Material.Ior != 1)
	{
		vec3	V = normalize(Camera.Position - Frag.Position);
		float	NdV = dot(Frag.Normal, V);
		if (NdV < 0) {
			Frag.Normal = -Frag.Normal;
			NdV = -NdV;
		}
		vec3	fresnel = Fresnel(NdV, F0(Frag.Material.Ior));
		vec2	refractFactor = vec2((1 - Frag.Depth) * max(fresnel.x, max(fresnel.y, fresnel.z)));
		vec2	refractDir = (mat3(Camera.Matrix.View) * normalize(refract(V, Frag.Normal, 1.0 / Frag.Material.Ior))).xy;
		refract_UV = refractDir * refractFactor + Frag.UV;
		refract_UV = warpUV(vec2(0), vec2(1), refract_UV);
	}
	vec3	Back_Color = sampleLod(Texture.Back.Color, refract_UV, Frag.Material.Roughness).rgb;
	vec3	Back_Emitting = sampleLod(Texture.Back.Emitting, refract_UV, Frag.Material.Roughness).rgb;
	vec3	Back_Normal = texture(opaqueBackNormal, refract_UV).rgb;
	if (Frag.Material.Alpha == 0) {
		return ;
	}
	vec3 refractionColor = mix(vec3(1), Frag.Material.Albedo, Frag.Material.Alpha);
	Out.Color.rgb = mix(Back_Color * refractionColor, 
						vec3(0),
						Frag.Material.Alpha);
	Out.Emitting.rgb = mix(Back_Emitting * refractionColor, 
						vec3(0),
						Frag.Material.Alpha);
}


)""