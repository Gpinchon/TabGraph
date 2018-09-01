#version 430
#pragma optionNV (unroll all)
#define	KERNEL_SIZE 64
#define M_PI 3.1415926535897932384626433832795

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_AO;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;

uniform vec3		in_CamPos;

uniform t_Environment	Environment;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec4	out_Emitting;

vec4	sampleLod(in samplerCube texture, in vec3 uv, in float value)
{
	return textureLod(texture, uv, value * textureQueryLevels(texture));
}

vec4	sampleLod(in sampler2D texture, in vec2 uv, in float value)
{
	return textureLod(texture, uv, value * textureQueryLevels(texture));
}

float	Env_Specular(in float NdV, in float NdL, in float roughness)
{
	float a2 = roughness * roughness;
	float G_V = NdV + sqrt( (NdV - NdV * a2) * NdV + a2 );
	float G_L = NdL + sqrt( (NdL - NdL * a2) * NdL + a2 );
	return (1 / (G_V * G_L));
}

/* float	Env_Specular(in float NdV, in float roughness)
{
	float a2 = roughness * roughness;
	float NdVa2 = NdV * a2;
	float G_V = NdV + sqrt((NdV - NdVa2) * NdV + a2 );
	float G_L = -NdV + sqrt((-NdV + NdVa2) * -NdV + a2 );
	return (1 / (G_V * G_L));
} */

float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	den = (alpha - 1) + 1;
	float	D = (alpha / (M_PI * den * den));
	float	alpha2 = alpha * alpha;
	float	G = (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
	return (D * G);
}

void main()
{
	const vec3		brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
	const vec3		envGammaCorrection = vec3(2.2); //For optimization, not meant to be set
	out_Color.a = 1;
	out_Emitting.a = 1;
	gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
	const vec3	EnvDiffuse = texture(Environment.Diffuse, frag_Cube_UV).rgb;
	const vec4	Albedo = texture(in_Texture_Albedo, frag_UV);
	const vec3	Fresnel = texture(in_Texture_Fresnel, frag_UV).rgb;
	const vec3	Emitting = texture(in_Texture_Emitting, frag_UV).rgb;
	const vec3	Material_Values = texture(in_Texture_Material_Values, frag_UV).xyz;
	const vec2	BRDF = texture(in_Texture_BRDF, frag_UV).xy;
	const vec3	Normal = texture(in_Texture_Normal, frag_UV).xyz;
	const vec3	Position = texture(in_Texture_Position, frag_UV).xyz;
	float		AO = 1 - texture(in_Texture_AO, frag_UV).r;
	
	vec3	V = normalize(in_CamPos - Position);
	vec3	R = reflect(V, Normal);
	float	Roughness = Material_Values.x;
	float	Metallic = Material_Values.y;

	vec3	diffuse = AO * (sampleLod(Environment.Diffuse, -Normal, Roughness + 0.9).rgb
			+ texture(Environment.Irradiance, -Normal).rgb);
	vec3	reflection = sampleLod(Environment.Diffuse, R, Roughness * 2.f).rgb;
	vec3	specular = texture(Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;

	reflection *= Fresnel;

	float	brightness = 0;

	if (Albedo.a == 0) {
		out_Color.rgb = EnvDiffuse;
		brightness = dot(pow(out_Color.rgb, envGammaCorrection), brightnessDotValue);
		out_Emitting.rgb = max(vec3(0), out_Color.rgb - 0.6) * min(1, brightness);
		return ;
	}
	out_Color.a = Albedo.a;
	out_Emitting.a = Albedo.a;

	float	NdV = max(0, dot(Normal, V));
	brightness = dot(pow(reflection_spec, envGammaCorrection), brightnessDotValue);
	reflection_spec *= brightness * min(Fresnel + 1, Fresnel * Env_Specular(NdV, Roughness));
	specular *= Fresnel * BRDF.x + mix(vec3(1), Fresnel, Metallic) * BRDF.y;
	specular += reflection_spec;
	diffuse *= Albedo.rgb * (1 - Metallic);

	out_Color.rgb = specular + diffuse + reflection;
	out_Color.rgb = mix(EnvDiffuse, out_Color.rgb, Albedo.a);
	out_Emitting.rgb = max(vec3(0), out_Color.rgb - 1) + Emitting;
}
