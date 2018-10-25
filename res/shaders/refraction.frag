uniform sampler2D	opaqueBackColor;
uniform sampler2D	opaqueBackEmitting;

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

vec3	Fresnel(in float factor, in vec3 F0, in float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
}

void	ApplyTechnique()
{
	vec2	refract_UV = Frag.UV;

	if (Frag.Material.Ior > 1)
	{
		vec3	V = normalize(Camera.Position - Frag.Position);
		float	NdV = dot(Frag.Normal, V);
		if (NdV < 0) {
			Frag.Normal = -Frag.Normal;
			NdV = -NdV;
		}
		vec3	fresnel = Fresnel(NdV, Frag.Material.Specular, Frag.Material.Roughness);
		vec2	refractFactor = (vec2(1 - Frag.Depth) * (fresnel.x + fresnel.y + fresnel.z) / 3.f) * 0.25f;
		vec2	refractDir = (mat3(Camera.Matrix.View) * normalize(refract(V, Frag.Normal, 1.0 / Frag.Material.Ior))).xy;
		refract_UV = refractDir * refractFactor + Frag.UV;
		refract_UV = warpUV(vec2(0), vec2(1), refract_UV);
	}
	vec3	Back_Color = sampleLod(opaqueBackColor, refract_UV, Frag.Material.Roughness).rgb;
	vec3	Back_Emitting = sampleLod(opaqueBackEmitting, refract_UV, Frag.Material.Roughness).rgb;
	if (Out.Color.a == 0) {
		Out.Color = vec4(Back_Color, 1);
		Out.Emitting = Back_Emitting;
		return ;
	}
	Back_Color = mix(Back_Color, Back_Color * Frag.Material.Albedo, Frag.Material.Alpha);
	Back_Emitting = mix(Back_Emitting, Back_Emitting * Frag.Material.Albedo, Frag.Material.Alpha);
	Out.Color.rgb += Back_Color * (1 - Frag.Material.Alpha);
	Out.Color.a = 1;
	Out.Emitting.rgb += Back_Emitting * (1 - Frag.Material.Alpha);
}