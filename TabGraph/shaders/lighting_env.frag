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

vec3	Fresnel(in float factor, in vec3 f0, in float alpha)
{
	return ((max(vec3(1 - alpha), f0)) * pow(max(0, 1 - factor), 5) + f0);
}

vec3	Fresnel(in float cosT, in vec3 f0)
{
  return (f0 + (1 - f0) * pow(1 - cosT, 5));
}

#define SSR() (textureLod(SSRResult, TexCoord(), 0))

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

void	Lighting()
{
	vec3	V = normalize(Camera.Position - WorldPosition());
	vec3	N = WorldNormal();
#ifdef TRANSPARENT
	float	NdV = dot(N, V);
	if (Opacity() < 1 && NdV < 0) {
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

	vec2	brdf = BRDF(NdV);
	vec4	ssrResult = SSR();
	vec3	diffuse = (1 - AO()) * texture(Texture.Environment.Irradiance, -N).rgb;
	vec3	envReflection = 
	//textureLod(Texture.Environment.Diffuse, R, sqrt(Alpha()) * 2.f * textureQueryLevels(Texture.Environment.Diffuse)).rgb;
	sampleLod(Texture.Environment.Diffuse, R, sqrt(Alpha()) * 2.f).rgb;
	vec3	envIrradiance =
	//textureLod(Texture.Environment.Irradiance, R, Alpha() * textureQueryLevels(Texture.Environment.Irradiance)).rgb;
	sampleLod(Texture.Environment.Irradiance, R, Alpha()).rgb;
	vec3	fresnel = Fresnel(NdV, F0(), Alpha());
	vec3	specularIntensity = min(fresnel, fresnel * Env_Specular(NdV, Alpha()));
	vec3	ENVSpecular = envIrradiance * (fresnel * brdf.x + brdf.y);// * Luminance(envIrradiance);
	vec3	SSRSpecular = ssrResult.rgb;// * Luminance(ssrResult.rgb);
	vec3	specular;
	vec3	reflection;
	reflection = mix(envReflection, ssrResult.rgb, ssrResult.a) * fresnel;
	specular = mix(ENVSpecular, SSRSpecular, ssrResult.a) * specularIntensity;
	specular *= Luminance(pow(specular, envGammaCorrection));
	//specular = envIrradiance * Luminance(envIrradiance);// * min(fresnel, fresnel * Env_Specular(NdV, Alpha()));
	//specular += ssrResult.rgb * Luminance(ssrResult.rgb) * ssrResult.a;// * min(fresnel, fresnel * Env_Specular(NdV, Alpha()));
	//specular *= fresnel * brdf.x + brdf.y;
	//diffuse *= (1 - fresnel) * CDiff();
	diffuse *= CDiff();

	float	alpha = Opacity() + max(specular.r, max(specular.g, specular.b));
	alpha = min(1, alpha);
	vec3 outColor = BackColor().rgb;
	//Out.Color.rgb += mix(envReflection, ssrResult.rgb * fresnel, ssrResult.a) * alpha;
	outColor += (specular + reflection) * alpha;
	outColor += (diffuse + Emissive()) * alpha;
#ifndef TRANSPARENT
	vec3	EnvDiffuse = texture(Texture.Environment.Diffuse, CubeTexCoord()).rgb;
	outColor += EnvDiffuse * floor(Depth()) * (1 - alpha);
#endif
	SetBackColor(vec4(outColor, 1));
	SetBackEmissive(BackEmissive() + max(vec3(0), BackColor().rgb - 1) + Emissive());
}

)""
