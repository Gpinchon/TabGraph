R""(
const vec3		envGammaCorrection = vec3(2.2);
uniform sampler2D	SSRResult;

float	Env_Specular(in float NdV, in float alpha)
{
	float	alpha2 = alpha * alpha;
	float	den = (alpha2 - 1) + 1;
	float	D = alpha2 / (M_PI * den * den);
	float	k = alpha / 2.f;
	float	denom = NdV * (1.f - k) + k;
	float	G = NdV / denom;
	return (max(D * G, 0));
}

vec3	Fresnel(in float factor, in vec3 F0, in float alpha)
{
	return ((max(vec3(1 - alpha), F0)) * pow(max(0, 1 - factor), 5) + F0);
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

vec4	SSR()
{
	return texture(SSRResult, Frag.UV, 0);
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

void	ApplyTechnique()
{
	Frag.AO = 1 - Frag.AO;
	vec3	V = normalize(Camera.Position - Frag.Position);
	vec3	N = Frag.Normal;
#ifdef TRANSPARENT
	float	NdV = dot(N, V);
	if (Frag.Opacity < 1 && NdV < 0) {
		N = -N;
		NdV = -NdV;
	}
	else {
		NdV = max(0, dot(N, V));
	}
#else
	float	NdV = max(0, dot(N, V));
#endif //TRANSPARENT
	vec3	R = reflect(V, N);

	vec2	brdf = BRDF(NdV, Frag.BRDF.Alpha);
	vec4	ssrResult = SSR();
	vec3	diffuse = Frag.AO * texture(Texture.Environment.Irradiance, -N).rgb;
	vec3	envReflection = sampleLod(Texture.Environment.Diffuse, R, sqrt(Frag.BRDF.Alpha) * 2.f).rgb;
	vec3	envIrradiance = sampleLod(Texture.Environment.Irradiance, R, Frag.BRDF.Alpha).rgb;
	vec3	fresnel = Fresnel(NdV, Frag.BRDF.F0, Frag.BRDF.Alpha);
	vec3	specularIntensity = min(fresnel, fresnel * Env_Specular(NdV, Frag.BRDF.Alpha));
	vec3	ENVSpecular = envIrradiance * (fresnel * brdf.x + brdf.y);// * Luminance(envIrradiance);
	vec3	SSRSpecular = ssrResult.rgb;// * Luminance(ssrResult.rgb);
	vec3	specular;
	vec3	reflection;
	reflection = mix(envReflection, ssrResult.rgb, ssrResult.a) * fresnel;
	specular = mix(ENVSpecular, SSRSpecular, ssrResult.a) * specularIntensity;
	specular *= Luminance(pow(specular, envGammaCorrection));
	//specular = envIrradiance * Luminance(envIrradiance);// * min(fresnel, fresnel * Env_Specular(NdV, Frag.BRDF.Alpha));
	//specular += ssrResult.rgb * Luminance(ssrResult.rgb) * ssrResult.a;// * min(fresnel, fresnel * Env_Specular(NdV, Frag.BRDF.Alpha));
	//specular *= fresnel * brdf.x + brdf.y;
	//diffuse *= (1 - fresnel) * Frag.BRDF.CDiff;
	diffuse *= Frag.BRDF.CDiff;

	float	alpha = Frag.Opacity + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);
	//Out.Color.rgb += mix(envReflection, ssrResult.rgb * fresnel, ssrResult.a) * alpha;
	Out.Color.rgb += (specular + reflection) * alpha;
	Out.Color.rgb += (diffuse + Frag.Emitting) * alpha;
#ifndef TRANSPARENT
	vec3	EnvDiffuse = texture(Texture.Environment.Diffuse, Frag.CubeUV).rgb;
	Out.Color.rgb += EnvDiffuse * floor(Frag.Depth) * (1 - alpha);
#endif
	Out.Color.a = 1;
	Out.Emitting.rgb += max(vec3(0), Out.Color.rgb - 1) + Frag.Emitting;
}

)""
