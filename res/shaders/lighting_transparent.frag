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

float	Env_Specular(in float NdV, in float roughness)
{
	float	alpha = roughness * roughness;
	float	den = (alpha - 1) + 1;
	float	D = (alpha / (M_PI * den * den));
	float	alpha2 = alpha * alpha;
	float	G = (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
	return (max(D * G, 0));
}

vec4	sampleLod(sampler2D texture, vec2 uv, float value)
{
	float factor = textureSize(texture, 0).x / 512.f;
	return textureLod(in_Back_Color, uv, value * factor);
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
	vec3	Back_Color = texture(in_Back_Color, frag_UV).xyz;
	vec3	Back_Bright = texture(in_Back_Bright, frag_UV).xyz;
	float	brightness = 0;
	if (Albedo.a == 0) {
		out_Color.rgb = Back_Color;
		out_Emitting.rgb = Back_Bright;
		return ;
	}
	float	Roughness = Material_Values.x;
	float	Metallic = Material_Values.y;
	float	AO = Material_Values.z;
	AO = clamp(1 - AO, 0, 1);

	vec3	diffuse = AO * (textureLod(Environment.Diffuse, -Normal, Roughness + 9).rgb
			+ textureLod(Environment.Irradiance, -Normal, Roughness * 4.f).rgb);
	vec3	V = normalize(in_CamPos - Position);
	float	NdV = max(0, dot(Normal, V));
	vec3	R = reflect(V, Normal);
	vec3	reflection = textureLod(Environment.Diffuse, R, Roughness * 12.f).rgb * Fresnel;
	vec3	specular = textureLod(Environment.Irradiance, R, Roughness * 10.f).rgb;
	vec3	reflection_spec = pow(textureLod(Environment.Diffuse, R, Roughness * 10.f + 3.5).rgb, vec3(2.2));

	//vec3	viewNormal = refract(V, -Normal, 1.f / BRDF.z);
	vec3	viewNormal = (in_ViewMatrix * vec4(Normal, 1)).xyz;
	float	refractionValue = (Fresnel.x + Fresnel.y + Fresnel.z) / 3.f;
	Back_Color = sampleLod(in_Back_Color, frag_UV - (viewNormal.xy * refractionValue), 0.5).xyz;
	//Back_Color = textureLod(in_Back_Color, frag_UV - (viewNormal.xy * refractionValue), Roughness * 10.f).xyz;
	Back_Bright = sampleLod(in_Back_Bright, frag_UV - (viewNormal.xy * refractionValue), Roughness).xyz;
	//Back_Bright = textureLod(in_Back_Bright, frag_UV - (viewNormal.xy * refractionValue), Roughness * 10.f).xyz;

	brightness = dot(reflection_spec, vec3(0.299, 0.587, 0.114));
	reflection_spec *= brightness * min(Fresnel + 1, Fresnel * Env_Specular(NdV, Roughness));
	specular *= Fresnel * BRDF.x + mix(vec3(1), Fresnel, Metallic) * BRDF.y;
	specular += reflection_spec;
	diffuse *= Albedo.rgb * (1 - Metallic);
	Albedo.a += dot(specular, specular);
	Albedo.a = min(1, Albedo.a);

	out_Color.rgb = Emitting.rgb + specular + diffuse + reflection;
	out_Color.rgb = mix(Back_Color, out_Color.rgb, Albedo.a);
	brightness = dot(pow(out_Color.rgb, vec3(2.2)), vec3(0.299, 0.587, 0.114));
	out_Emitting.rgb = max(vec3(0), out_Color.rgb - 0.6) * min(1, brightness) + Emitting.rgb;
}
