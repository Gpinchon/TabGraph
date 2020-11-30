R""(

#define OPAQUEPASS 1
#define TRANSPARENTPASS 2

layout(location = 0) out vec4	out_0;
layout(location = 1) out vec4	out_1;
layout(location = 2) out vec4	out_2;

uniform int			RenderPass;
uniform sampler2D	DiffuseTexture;
uniform sampler2D	SpecularTexture;
uniform sampler2D	AOTexture;
uniform sampler2D	NormalTexture;
uniform usampler2D	IDTexture;

vec4 TransparencyColor(const in vec4 Ca, const in vec4 Cb) {
	vec3 denom = Ca.rgb * Ca.a + Cb.rgb * Cb.a * (1 - Ca.a);
	float alpha = (Ca.a + Cb.a * (1 - Ca.a));
	vec3 color = denom / alpha;
	if (alpha > 0)
		return vec4(color, alpha);
	return vec4(0);
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

void WritePixel(vec4 premultipliedReflect)
{
	//float z = gl_FragCoord.z;
	//float maxVal = max(color.r, max(color.g, color.b));
	//float a = max(min(1.0, maxVal * color.a), color.a);
	//vec4 premultipliedReflect = color * color.a;
	
	//float weight = a * clamp(0.03 / (1e-5 + pow(gl_FragCoord.z / 200, 4.0)), 1e-2, 3e3);
	//float weight = color.a * max(10e-2, min(30e3, 0.03 / (10e-5 + pow(gl_FragCoord.z / 200, 4.0))));
	//float weight = pow(a + 0.01, 4.0) + max(1e-2, min(3.0 * 1e3, 100.0 / (1e-5 + pow(abs(z) / 10.0, 3.0) + pow(abs(z) / 200.0, 6.0))));
	//float weight = clamp(pow(min(1.0, premultipliedReflect.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
	float a = min(1.0, premultipliedReflect.a) * 8.0 + 0.01;
    float b = -gl_FragCoord.z * 0.95 + 1.0;
    float weight = clamp(a * a * a * 1e8 * b * b * b, 1e-2, 3e2);
    out_0 = premultipliedReflect * weight;
    out_1 = vec4(premultipliedReflect.a);
}

void Reconstruct() {
	vec2	uv = (gl_FragCoord.xy) / Resolution.xy;
	vec4[4] diffuseSamples = GatherTexture(DiffuseTexture, uv);
	vec4[4] specularSamples = GatherTexture(SpecularTexture, uv);
	vec4[4] normalSamples = GatherTexture(NormalTexture, uv);
	vec4[4] AOSamples = GatherTexture(AOTexture, uv);
	uint[4] IDSamples = GatherIDTexture(IDTexture, uv);

	vec3	V = normalize(Camera.Position - WorldPosition());
	vec3	N = WorldNormal();
	float	NdV = max(dot(N, V), 0);
	vec3	R = reflect(V, N);
	float	alphaSqrt = min(sqrt(Alpha()) * 2.5f, 1.f);
	vec3	fresnel = Fresnel(NdV, F0(), Alpha());

	vec3	envDiffuse = EnvironmentDiffuse(alphaSqrt);
	vec3	envReflection = EnvironmentReflection(fresnel, R, alphaSqrt);
	vec4	thisColor = vec4(0);
	float	thisTotalWeight = 0;
	for (int i = 0; i < 4; ++i) {
		if (IDSamples[i] == DrawID) {
			vec3 thisEnvDiffuse = (envDiffuse * CDiff()) * (1 - AOSamples[i].r);
			vec3 diffuse = thisEnvDiffuse + diffuseSamples[i].rgb * CDiff();
			vec3 reflection = mix(envReflection, specularSamples[i].xyz * fresnel, specularSamples[i].a);
			vec3 specular = reflection + diffuseSamples[i].rgb * diffuseSamples[i].a * fresnel;
			//vec3 specular = (reflection + diffuseSamples[i].rgb * specularIntensity) * F0();
			float weight = max(dot(normalize(decodeNormal(normalSamples[i].xy)), WorldNormal()) + 0.25, 0);
			thisTotalWeight += weight;
			thisColor += vec4(diffuse + specular, Luminance(specular)) * weight;
		}
	}
	thisColor /= max(1, thisTotalWeight);
	if (thisTotalWeight > 0) {
		thisColor.a = min(Opacity() + thisColor.a, 1);
	}
	else
		thisColor = vec4(envDiffuse * CDiff() * (1 - AO()) + envReflection, Opacity());
	if (RenderPass == TRANSPARENTPASS) {
		if (Opacity() < 1) {
			out_2 = max(thisColor - 1, 0) + vec4(Emissive(), thisColor.a);
			thisColor.rgb += Emissive();
			WritePixel(thisColor * thisColor.a);
		}
		else discard;
	}
	else if (Opacity() == 1) {
		out_1 = max(thisColor - 1, 0) + vec4(Emissive(), thisColor.a);
		thisColor.rgb += Emissive();
		out_0 = thisColor;
	}
	else discard;
	//out_Alpha = thisColor.a;
	//out_Emissive += clamp(out_Color.rgb - 1, 0, 1);
	//if (out_Color.a < 0.003)
	//	discard;
}

bool	CheckOpacity()
{
	if (Opacity() >= 0.99f) {
		SetOpacity(1);
		return false;
	}
	if (StandardValues.OpacityMode == OPAQUE) {
		SetOpacity(1);
		return false;
	}
	if (StandardValues.OpacityMode == MASK) {
		if(Opacity() > StandardValues.OpacityCutoff) {
			SetOpacity(1);
			return false;
		}
		return true;
	}
	return Opacity() < 0.003;
}

void ComputeColor() {
	bool toDiscard = CheckOpacity();
	Reconstruct();
	if (toDiscard)
		discard;
}

)""