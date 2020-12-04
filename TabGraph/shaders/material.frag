R""(

#define OPAQUEPASS 1
#define TRANSPARENTPASS 2

layout(location = 0) out vec4	out_0;
layout(location = 1) out vec4	out_1;
layout(location = 2) out vec4	out_2;

uniform int			RenderPass;
uniform sampler2D	DiffuseTexture;
//uniform sampler2D	SpecularTexture;
uniform sampler2D	ReflectionTexture;
uniform sampler2D	AOTexture;
uniform sampler2D	NormalTexture;
uniform usampler2D	IDTexture;
in float CameraSpaceDepth;

vec4 TransparencyColor(const in vec4 Ca, const in vec4 Cb) {
	vec3 denom = Ca.rgb * Ca.a + Cb.rgb * Cb.a * (1 - Ca.a);
	float alpha = (Ca.a + Cb.a * (1 - Ca.a));
	vec3 color = denom / alpha;
	if (alpha > 0)
		return vec4(color, alpha);
	return vec4(0);
}

ivec2[2][5] SampleOffset = ivec2[2][5](
	ivec2[5](
		ivec2( 0,  0),
		ivec2(-1,  0),
		ivec2( 1,  0),
		ivec2( 0, -1),
		ivec2( 0,  1)
	),
	ivec2[5](
		ivec2( 0,  0),
		ivec2(-1, -1),
		ivec2( 1,  1),
		ivec2( 1, -1),
		ivec2(-1,  1)
	)
);

uint	OffsetIndex(ivec2 iuv) {
	bool XEven = iuv.x % 2 == 0;
	bool YEven = iuv.y % 2 == 0;
	uint drawIndex = DrawID % 4;
	if (drawIndex / 2 == 0 && drawIndex % 2 == 0) {
		if (!XEven && !YEven)
			return 1;
		else return 0;
	}
	else if (drawIndex / 2 == 0 && drawIndex % 2 == 1) {
		if (XEven && YEven)
			return 1;
		else return 0;
	}
	else if (drawIndex / 2 == 1 && drawIndex % 2 == 0) {
		if (!XEven && YEven)
			return 1;
		else return 0;
	}
	else if (drawIndex / 2 == 1 && drawIndex % 2 == 1) {
		if (XEven && !YEven)
			return 1;
		else return 0;
	}
	return 0;
}

vec4[5] SampleTexture(sampler2D tex, vec2 uv) {
	vec4[5] samples;
	ivec2 iuv = ivec2(uv * textureSize(tex, 0));
	ivec2[5] offsets = SampleOffset[OffsetIndex(iuv)];
	samples[0] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[0]);
	samples[1] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[1]);
	samples[2] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[2]);
	samples[3] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[3]);
	samples[4] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[4]);
	return samples;
}

uint[5] SampleIDTexture(usampler2D tex, vec2 uv) {
	uint[5] samples;
	ivec2 iuv = ivec2(uv * textureSize(tex, 0));
	ivec2[5] offsets = SampleOffset[OffsetIndex(iuv)];
	samples[0] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[0]).r;
	samples[1] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[1]).r;
	samples[2] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[2]).r;
	samples[3] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[3]).r;
	samples[4] = texelFetchOffset(tex, ivec2(iuv), 0, offsets[4]).r;
	return samples;
}

vec4[4] GatherTexture(sampler2D tex, vec2 uv) {
	vec4 x = textureGather(tex, uv, 0);
	vec4 y = textureGather(tex, uv, 1);
	vec4 z = textureGather(tex, uv, 2);
	vec4 w = textureGather(tex, uv, 3);
 	return vec4[4](
		vec4(x[0], y[0], z[0], w[0]),
		vec4(x[1], y[1], z[1], w[1]),
		vec4(x[2], y[2], z[2], w[2]),
		vec4(x[3], y[3], z[3], w[3])
	);
}

uint[4] GatherIDTexture(usampler2D tex, vec2 uv) {
	uvec4 i = textureGather(tex, uv, 0);
 	return uint[4](
		i[0], i[1], i[2], i[3]
	);
}

#define Fresnel(factor, f0, alpha) (f0 + (max(f0, 1 - alpha) - f0) * pow(1 - max(factor, 0), 5))

vec3 EnvironmentReflection(const in vec3 fresnel, const in vec3 R, const in float alphaSqrt) {
	return sampleLod(Environment.Diffuse, R, alphaSqrt).rgb * fresnel;
}

vec3 EnvironmentDiffuse(const in float alphaSqrt) {
	return (1 - AO()) * sampleLod(Environment.Irradiance, -WorldNormal(), (0.8 + 0.2 * alphaSqrt)).rgb;
}

vec3 EnvironmentSpecular(const in vec3 fresnel, const in vec3 R, const in float alphaSqrt, const in vec2 brdf) {
	vec3	irradianceFactor = fresnel * brdf.x + brdf.y;
	vec3	specSample = sampleLod(Environment.Irradiance, R, (0.8 + 0.2 * alphaSqrt)).rgb;
	return (1 - AO()) * specSample * irradianceFactor * (1 - F0());
}

void WritePixel(vec3 color, float alpha, float wsZ) {
    float ndcZ = 2.0 * wsZ - 1.0;
    // linearize depth for proper depth weighting
    //See: https://stackoverflow.com/questions/7777913/how-to-render-depth-linearly-in-modern-opengl-with-gl-fragcoord-z-in-fragment-sh
    //or: https://stackoverflow.com/questions/11277501/how-to-recover-view-space-position-given-view-space-depth-value-and-ndc-xy
    float linearZ = CameraSpaceDepth;//(Camera.Matrix.Projection[2][2] + 1.0) * wsZ / (Camera.Matrix.Projection[2][2] + ndcZ);
    float tmp = (1.0 - linearZ) * alpha;
    //float tmp = (1.0 - wsZ * 0.99) * alpha * 10.0; // <-- original weighting function from paper #2
    float w = clamp(tmp * tmp * tmp * tmp * tmp * tmp, 0.0001, 1000.0);
    out_0 = vec4(color * alpha, alpha) * w;
    out_1 = vec4(alpha);
}

void WritePixel(vec3 premultipliedReflect, float coverage)
{
	float znear = 0.1f;
	float zfar = 1000.f;
	float z = abs(CameraSpaceDepth);
	//z = (z-znear)/(zfar-znear);
	//float z = CameraSpaceDepth / gl_FragCoord.w / 10.f;
	//float z = 1-gl_FragCoord.z;
	//float w = pow(coverage + 0.01f, 4.0f) + max(0.01f, min(3000.0f, 0.3f / (0.00001f + pow(abs(z) / 200.0f, 4.0f))));
	float w = clamp(pow(abs(1 / z), 4.f) * coverage * coverage, 1e-5, 1e5);
	//float a = min(3 * pow(10, 3), 0.03 / (pow(10, -5) + pow(z / 200.f, 4.f)));
	//float w = coverage * max(pow(10, -2), a);// * (1 - gl_FragCoord.z);
	//float z = distance(WorldPosition(), Camera.Position);
	//float z = (1 - gl_FragCoord.z);// * (1 + 500 * coverage);
	//float w = clamp(pow(min(1.0, coverage * 10.0) + 0.01, 3.0) * 1e8 * pow(z, 3.0), 1e-2, 3e3);
	/*float tmp = 1.f - gl_FragCoord.z * 0.99; tmp *= tmp * tmp * 1e4;
    tmp = clamp(tmp, 1e-3, 1.0);
	float w = clamp(coverage * tmp, 1e-3, 1.5e2);*/
	//float w = coverage * max(pow(10, -2), 3 * pow(10, 3) * pow(1.f - gl_FragCoord.z, 30));
	out_0 = vec4(premultipliedReflect, coverage) * w;
	out_1 = vec4(coverage);
}

void Reconstruct() {
	vec4[5] diffuseSamples = SampleTexture(DiffuseTexture, ScreenTexCoord());
	vec4[5] reflectionSamples = SampleTexture(ReflectionTexture, ScreenTexCoord());
	vec4[5] normalSamples = SampleTexture(NormalTexture, ScreenTexCoord());
	vec4[5] AOSamples = SampleTexture(AOTexture, ScreenTexCoord());
	uint[5] IDSamples = SampleIDTexture(IDTexture, ScreenTexCoord());

	vec3	V = normalize(Camera.Position - WorldPosition());
	vec3	N = WorldNormal();
	float	NdV = max(dot(N, V), 0);
	vec3	R = reflect(V, N);
	float	alphaSqrt = sqrt(Alpha());
	float	sampleLOD = min(alphaSqrt * 2.f, 1.f);
	vec2	brdf = BRDF(NdV, alphaSqrt);
	vec3	fresnel = min(F0() * brdf.x + brdf.y, 1);
	float	SpecularPower = exp2(10 * (1 - alphaSqrt) + 1);
	
	
	vec3	envDiffuse = EnvironmentDiffuse(sampleLOD);
	vec3	envReflection = EnvironmentReflection(fresnel, R, sampleLOD);
	vec4	thisColor = vec4(0);
	float	thisTotalWeight = 0;
	for (int i = 0; i < 5; ++i) {
		if (IDSamples[i] == DrawID) {
			vec3 thisEnvDiffuse = (envDiffuse * CDiff()) * (1 - AOSamples[i].r);
			vec3 diffuse = thisEnvDiffuse + diffuseSamples[i].rgb * CDiff();
			vec3 reflection = mix(envReflection, reflectionSamples[i].xyz * fresnel, reflectionSamples[i].a);
			vec3 specular = diffuseSamples[i].rgb * diffuseSamples[i].a * fresnel;
			float weight = max(dot(normalize(decodeNormal(normalSamples[i].xy)), WorldNormal()), 0);
			thisTotalWeight += weight;
			thisColor += vec4(diffuse + reflection + specular, Luminance(specular + reflection)) * weight;
		}
	}
	thisColor /= max(1, thisTotalWeight);
	vec4 defaultColor = vec4(envDiffuse * CDiff() * (1 - AO()) + envReflection, Opacity() + Luminance(envReflection));
	thisColor = mix(defaultColor, thisColor, min(1, thisTotalWeight));
	if (thisTotalWeight > 0) {
		thisColor.a = min(Opacity() + thisColor.a, 1);
	}
	#if OPACITYMODE == BLEND
	if (RenderPass == TRANSPARENTPASS) {
		if (Opacity() < 1) {
			out_2 = max(thisColor - 1, 0) + vec4(Emissive(), thisColor.a);
			thisColor.rgb += Emissive();
			WritePixel(thisColor.rgb * thisColor.a, thisColor.a);
			//WritePixel(thisColor.rgb, thisColor.a, gl_FragCoord.z);
		}
		//else discard;
	}
	else if (Opacity() == 1) {
		out_1 = max(thisColor - 1, 0) + vec4(Emissive(), thisColor.a);
		thisColor.rgb += Emissive();
		out_0 = thisColor;
	}
	else discard;
	#else
	if (Opacity() == 1) {
		out_1 = max(thisColor - 1, 0) + vec4(Emissive(), thisColor.a);
		thisColor.rgb += Emissive();
		out_0 = thisColor;
	}
	#endif
}

bool	CheckOpacity()
{
	if (Opacity() >= 0.99f) {
		SetOpacity(1);
		return false;
	}
	#if OPACITYMODE == OPAQUE
		SetOpacity(1);
		return false;
	#endif
	#if OPACITYMODE == MASK
		if(Opacity() > StandardValues.OpacityCutoff) {
			SetOpacity(1);
			return false;
		}
		return true;
	#endif
	return Opacity() < 0.003;
}

void ComputeColor() {
	bool toDiscard = CheckOpacity();
	Reconstruct();
	if (toDiscard)
		discard;
}

)""