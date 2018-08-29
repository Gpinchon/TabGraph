#version 410
#pragma optionNV (unroll all)
#define	KERNEL_SIZE 64
#define M_PI 3.1415926535897932384626433832795

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;
uniform sampler2D	in_Back_Color;
uniform sampler2D	in_Back_Bright;

uniform vec3		in_CamPos;
uniform mat4		in_ViewMatrix;

uniform t_Environment	Environment;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec4	out_Emitting;

vec4	sampleLod(samplerCube texture, vec3 uv, float value)
{
	value = clamp(value, 0, 1);
	float factor = floor(log2(float(textureSize(texture, 0).x)));
	return textureLod(texture, uv, value * factor);
}

vec4	sampleLod(sampler2D texture, vec2 uv, float value)
{
	value = clamp(value, 0, 1);
	float factor = floor(log2(float(textureSize(texture, 0).x)));
	return textureLod(texture, uv, value * factor);
}

float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	den = (alpha - 1) + 1;
	float	D = (alpha / (M_PI * den * den));
	float	alpha2 = alpha * alpha;
	float	G = (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
	return (max(D * G, 0));
}

void main()
{
	out_Color.a = 1;
	out_Emitting.a = 1;
	float	Depth = gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
	vec3	EnvDiffuse = texture(Environment.Diffuse, frag_Cube_UV).rgb;
	vec3	EnvIrradiance = texture(Environment.Diffuse, frag_Cube_UV).rgb;
	vec4	Albedo = texture(in_Texture_Albedo, frag_UV);
	vec3	Fresnel = texture(in_Texture_Fresnel, frag_UV).rgb;
	vec4	Emitting = texture(in_Texture_Emitting, frag_UV);
	vec4	Material_Values = texture(in_Texture_Material_Values, frag_UV);
	vec4	BRDF = texture(in_Texture_BRDF, frag_UV);
	vec3	Normal = normalize(texture(in_Texture_Normal, frag_UV).xyz);
	vec3	Position = texture(in_Texture_Position, frag_UV).xyz;
	float	Roughness = Material_Values.x;
	float	Metallic = Material_Values.y;
	float	AO = Material_Values.z;
	float	Ior = BRDF.z;
	float	brightness = 0;

	vec3	V = normalize(in_CamPos - Position);
	float	NdV = dot(Normal, V);
	if (NdV < 0) {
		Normal = -Normal;
		NdV = dot(Normal, V);
	}
	NdV = max(0, NdV);
	vec2	refract_UV = frag_UV;
	if (Ior > 1)
	{
		vec2	refractFactor = vec2(0.0125f) + (Fresnel.x + Fresnel.y + Fresnel.z) / 3.f * 0.0125f;
		vec2	refractDir = (mat3(in_ViewMatrix) * normalize(refract(V, Normal, 1.0 / Ior))).xy;
		refract_UV = frag_UV + refractDir * refractFactor;
		if (refract_UV.x > 1) {
			float refractOffset = (refract_UV.x - 1);
			refractDir.x = mix(refractDir.x, -refractDir.x, refractOffset * refractFactor.x * textureSize(in_Back_Color, 0).x);
		}
		else if (refract_UV.x < 0) {
			float refractOffset = abs(refract_UV.x);
			refractDir.x = mix(refractDir.x, -refractDir.x, refractOffset * refractFactor.x * textureSize(in_Back_Color, 0).x);
		}
		if (refract_UV.y > 1) {
			float refractOffset = (refract_UV.y - 1);
			refractDir.y = mix(refractDir.y, -refractDir.y, refractOffset * refractFactor.y * textureSize(in_Back_Color, 0).y);
		}
		else if (refract_UV.y < 0) {
			float refractOffset = abs(refract_UV.y);
			refractDir.y = mix(refractDir.y, -refractDir.y, refractOffset * refractFactor.y * textureSize(in_Back_Color, 0).y);
		}
		refract_UV = frag_UV + refractDir * refractFactor;
		/* if (refract_UV.x > 1)
			refract_UV.x = 1 - (refract_UV.x - 1);
		else if (refract_UV.x < 0)
			refract_UV.x = 0 + abs(refract_UV.x);
		if (refract_UV.y > 1)
			refract_UV.y = 1 - (refract_UV.y - 1);
		else if (refract_UV.y < 0)
			refract_UV.y = 0 + abs(refract_UV.y); */
		//if (refract_UV.y > 1 || refract_UV.y < 0)
		//	refract_UV.y = frag_UV.y - refractDir.y * 0.01;
	}
	vec3	Back_Color = sampleLod(in_Back_Color, refract_UV, Roughness).rgb;
	vec3	Back_Bright = sampleLod(in_Back_Bright, refract_UV, Roughness).rgb;

	if (Albedo.a == 0) {
		out_Color.rgb = Back_Color;
		out_Emitting.rgb = Back_Bright;
		return ;
	}

	AO = clamp(1 - AO, 0, 1);

	vec3	diffuse = AO * (sampleLod(Environment.Diffuse, -Normal, Roughness + 0.1).rgb
			+ sampleLod(Environment.Irradiance, -Normal, Roughness).rgb);
	vec3	R = reflect(V, Normal);
	vec3	reflection = sampleLod(Environment.Diffuse, R, Roughness * 1.5f).rgb * Fresnel;
	vec3	specular = sampleLod(Environment.Irradiance, R, Roughness).rgb;
	vec3	reflection_spec = pow(sampleLod(Environment.Diffuse, R, Roughness + 0.1).rgb, vec3(2.2));

	brightness = dot(reflection_spec, vec3(0.299, 0.587, 0.114));
	reflection_spec *= brightness * min(Fresnel + 1, Fresnel * Env_Specular(NdV, Roughness));
	specular *= Fresnel * BRDF.x + mix(vec3(1), Fresnel, Metallic) * BRDF.y;
	specular += reflection_spec;
	diffuse *= Albedo.rgb * (1 - Metallic);
	Albedo.a += dot(specular, specular);
	//Albedo.a += Roughness;
	Albedo.a = min(1, Albedo.a);

	out_Color.rgb = specular + diffuse + reflection;
	out_Color.rgb = mix(Back_Color, out_Color.rgb, Albedo.a);
	brightness = dot(pow(out_Color.rgb, vec3(2.2)), vec3(0.299, 0.587, 0.114));
	out_Emitting.rgb = max(vec3(0), out_Color.rgb - 0.6) * min(1, brightness) + Emitting.rgb;
	out_Emitting.rgb = mix(Back_Bright, out_Emitting.rgb, Albedo.a);
}
