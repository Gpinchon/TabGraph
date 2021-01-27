R""(

#define OPAQUEPASS 1
#define TRANSPARENTPASS 2

layout(location = 0) out vec4	out_0;
layout(location = 1) out vec4	out_1;
layout(location = 2) out vec4	out_2;
layout(location = 3) out vec4	out_3;

uniform int			RenderPass;
uniform sampler2D	DiffuseTexture;
uniform sampler2D	ReflectionTexture;
uniform sampler2D	AOTexture;
uniform sampler2D	NormalTexture;
uniform usampler2D	IDTexture;
uniform sampler2D	OpaqueDepthTexture;
in float CameraSpaceDepth;

vec4 TransparencyColor(const in vec4 Ca, const in vec4 Cb) {
	vec3 denom = Ca.rgb * Ca.a + Cb.rgb * Cb.a * (1 - Ca.a);
	float alpha = (Ca.a + Cb.a * (1 - Ca.a));
	vec3 color = denom / alpha;
	if (alpha > 0)
		return vec4(color, alpha);
	return vec4(0);
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

#if OPACITYMODE == BLEND
float LinearDepth(const in float z)
{
	float	depth = z;
	vec2	uv = TexCoord();
	vec4	projectedCoord = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.Projection * projectedCoord;
	float A = Camera.Matrix.Projection[2].z;
    float B = Camera.Matrix.Projection[3].z;
	float zNear = - B / (1.0 - A);
    float zFar  =   B / (1.0 + A);
	zFar = isinf(zFar) ? 1000.f : zFar;
	return (-projectedCoord.z / projectedCoord.w + zNear) / (zFar - zNear);
}

float LinearDepth()
{
	float z = abs(CameraSpaceDepth);
	float A = Camera.Matrix.Projection[2].z;
    float B = Camera.Matrix.Projection[3].z;
	float zNear = - B / (1.0 - A);
    float zFar  =   B / (1.0 + A);
	return 0.5*(-A*z + B) / z + 0.5;
}

void WritePixel(vec3 premultipliedReflect, float coverage)
{
	float z = LinearDepth();
	float w = clamp(pow(z, -4), 6.1*1e-4, 1e5);

	//float w = clamp(pow(abs(1 / z), 4.f) * coverage * coverage, 6.1*1e-4, 1e5);

	out_0 = vec4(premultipliedReflect, coverage) * w;
	out_1 = vec4(1 - coverage);
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
#endif

ivec2 offset[13] = ivec2[13](
                                    ivec2(0, -2),
                    ivec2(-1, -1),	ivec2(0, -1),	ivec2(1, -1),
    ivec2(-2,  0),	ivec2(-1,  0),	ivec2(0,  0),	ivec2(1,  0),	ivec2(2,  0),
                    ivec2(-1,  1),	ivec2(0,  1),	ivec2(1,  1),
                                    ivec2(0,  2)
);

void Reconstruct() {
#if OPACITYMODE == BLEND
	float opaqueDepth = texture(OpaqueDepthTexture, ScreenTexCoord(), 0).r;
#endif
	vec4 diffuseSample = texture(DiffuseTexture, ScreenTexCoord());
	vec4 reflectionSample = texture(ReflectionTexture, ScreenTexCoord());
	vec4 normalSample = texture(NormalTexture, ScreenTexCoord());
	vec4 AOSample = texture(AOTexture, ScreenTexCoord());
	uint IDSample = texture(IDTexture, ScreenTexCoord()).r;


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
	vec4	thisColor = vec4(envDiffuse * CDiff() * (1 - AO()) + envReflection, Opacity() + Luminance(envReflection));
	if (IDSample == DrawID) {
		vec3 thisEnvDiffuse = (envDiffuse * CDiff()) * (1 - AOSample.r);
		vec3 diffuse = thisEnvDiffuse + diffuseSample.rgb * CDiff();
		vec3 reflection = mix(envReflection, reflectionSample.xyz * fresnel, reflectionSample.a);
		vec3 specular = diffuseSample.rgb * diffuseSample.a * fresnel;
		float weight = max(dot(normalSample.xyz, WorldNormal()), 0);
		vec4 color = vec4(diffuse + reflection + specular, min(1, Opacity() + Luminance(specular + reflection)));
		thisColor = color * weight + thisColor * (1 - weight);
	}

	#if OPACITYMODE == BLEND
	if (opaqueDepth < gl_FragCoord.z)
		discard;
	out_2 = vec4((max(thisColor.rgb - 1, 0) + Emissive()) * thisColor.a, thisColor.a);
	WritePixel((thisColor.rgb + Emissive()) * thisColor.a, thisColor.a);
	float	backDepth = LinearDepth(opaqueDepth);
	float	thisDepth = LinearDepth(gl_FragCoord.z);
	float	depthDiff = abs(backDepth - thisDepth);
	vec3	normal = WorldNormal();
	if (dot(normal, normalize(Camera.Position - WorldPosition())) < 0)
		normal = -normal;
	vec4	viewNormal = Camera.Matrix.View * vec4(normal, 0);
	vec3	viewV = normalize(-vec3(0, 0, distance(WorldPosition(), Camera.Position)));
	vec4	refractDir = vec4(refract(viewV, viewNormal.xyz, 1.f/ Ior()), 0);
		
	vec2	refractOffset = refractDir.xy * min(0.05, depthDiff);//min(0.025, depthDiff);

	out_3 = vec4(refractOffset, Alpha() * Opacity(), 0);
	#else
	out_1 = vec4(max(thisColor.rgb - 1, 0) + Emissive(), thisColor.a);
	out_0 = vec4(thisColor.rgb + Emissive(), thisColor.a);
	#endif
}

bool	CheckOpacity()
{
	if (Opacity() >= 0.95f) {
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