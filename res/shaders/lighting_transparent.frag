#version 430
#pragma optionNV (unroll all)
#define M_PI 3.1415926535897932384626433832795

precision lowp float;
precision lowp int;
precision lowp sampler2D;
precision lowp samplerCube;

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

uniform vec3		brightnessDotValue = vec3(0.299, 0.587, 0.114); //For optimization, not meant to be set
uniform vec3		envGammaCorrection = vec3(2.2); //For optimization, not meant to be set

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_AO;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Depth;
uniform sampler2D	in_Texture_BRDF;

uniform sampler2D	in_Back_Color;
uniform sampler2D	in_Back_Bright;

uniform vec3		in_CamPos;
uniform mat4		in_ViewMatrix;
uniform mat4		in_InvProjViewMatrix;

uniform t_Environment	Environment;

in vec2				frag_UV;
in vec3				frag_Cube_UV;

layout(location = 0) out vec4	out_Color;
layout(location = 1) out vec4	out_Emitting;

struct t_Material {
	vec4		Albedo;
	vec3		Specular;
	vec3		Emitting;
	float		Roughness;
	float		Metallic;
	float		Ior;
	float		AO;
};

struct t_Frag {
	float		Depth;
	vec3		Position;
	vec3		Normal;
	t_Material	Material;
};

t_Frag	Frag;

vec3	WorldPosition(vec2 UV)
{
	float	linearDepth = texture(in_Texture_Depth, UV).r * 2.0 - 1.0;
	vec2	coord = UV * 2.0 - 1.0;
	vec4	projectedCoord = vec4(coord, linearDepth, 1.0);
	projectedCoord = in_InvProjViewMatrix * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

vec3	WorldPosition()
{
	float	linearDepth = Frag.Depth * 2.0 - 1.0;
	vec2	coord = frag_UV * 2.0 - 1.0;
	vec4	projectedCoord = vec4(coord, linearDepth, 1.0);
	projectedCoord = in_InvProjViewMatrix * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

void	FillFrag()
{
	Frag.Depth = gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
	Frag.Position = WorldPosition();
	Frag.Normal = texture(in_Texture_Normal, frag_UV).xyz;
	Frag.Material.Albedo = texture(in_Texture_Albedo, frag_UV);
	Frag.Material.Specular = texture(in_Texture_Fresnel, frag_UV).xyz;
	Frag.Material.Emitting = texture(in_Texture_Emitting, frag_UV).xyz;
	vec3	Material_Values = texture(in_Texture_Material_Values, frag_UV).xyz;
	Frag.Material.Roughness = Material_Values.x;
	Frag.Material.Metallic = Material_Values.y;
	Frag.Material.Ior = Material_Values.z;
	Frag.Material.AO = texture(in_Texture_AO, frag_UV).r;
}

vec4	sampleLod(samplerCube texture, vec3 uv, float value)
{
	value = clamp(value, 0, 1);
	float factor = textureQueryLevels(texture);//floor(log2(float(textureSize(texture, 0).x)));
	return textureLod(texture, uv, value * factor);
}

vec4	sampleLod(sampler2D texture, vec2 uv, float value)
{
	value = clamp(value, 0, 1);
	float factor = textureQueryLevels(texture);//floor(log2(float(textureSize(texture, 0).x)));
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


/* float	Env_Specular(in float NdV, in float roughness)
{
	float a2 = roughness * roughness;
	float NdVa2 = NdV * a2;
	float G_V = NdV + sqrt((NdV - NdVa2) * NdV + a2 );
	float G_L = -NdV + sqrt((-NdV + NdVa2) * -NdV + a2 );
	return (1 / (G_V * G_L));
} */

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

float	map(in float value, in float low1, in float high1, in float low2, in float high2)
{
	return (low2 + (value - low1) * (high2 - low2) / (high1 - low1));
}

void main()
{
	out_Color.a = 1;
	out_Emitting.a = 1;
	FillFrag();
	vec3	Normal = texture(in_Texture_Normal, frag_UV).xyz;
	Frag.Material.AO = 1 - Frag.Material.AO;

	vec3	V = normalize(in_CamPos - Frag.Position);
	float	NdV = dot(Normal, V);
	if (NdV < 0) {
		Normal = -Normal;
		NdV = -NdV;
	}

	const vec2	BRDF = texture(in_Texture_BRDF, vec2(NdV, Frag.Material.Roughness)).xy;

	vec3	diffuse = Frag.Material.AO * (sampleLod(Environment.Diffuse, -Normal, Frag.Material.Roughness + 0.9).rgb
			+ texture(Environment.Irradiance, -Normal).rgb);
	diffuse *= Frag.Material.Albedo.a;
	vec3	R = reflect(V, Normal);
	vec3	reflection = sampleLod(Environment.Diffuse, R, Frag.Material.Roughness * 2.f).rgb;
	vec3	specular = texture(Environment.Irradiance, R).rgb;
	vec3	reflection_spec = reflection;

	reflection *= Frag.Material.Specular;

	vec2	refract_UV = frag_UV;
	if (Frag.Material.Ior > 1)
	{
		vec2	refractFactor = vec2(1 - Frag.Depth) * vec2(0.25f) + (Frag.Material.Specular.x + Frag.Material.Specular.y + Frag.Material.Specular.z) / 3.f * 0.0125f;
		vec2	refractDir = (mat3(in_ViewMatrix) * normalize(refract(V, Normal, 1.0 / Frag.Material.Ior))).xy;
		refract_UV = refractDir * refractFactor + frag_UV;
		refract_UV = warpUV(vec2(0), vec2(1), refract_UV);
	}
	vec3	Back_Color = sampleLod(in_Back_Color, refract_UV, Frag.Material.Roughness).rgb;
	vec3	Back_Bright = sampleLod(in_Back_Bright, refract_UV, Frag.Material.Roughness).rgb;

	if (Frag.Material.Albedo.a == 0) {
		out_Color.rgb = Back_Color;
		out_Emitting.rgb = Back_Bright;
		return ;
	}

	float	brightness = 0;
	brightness = dot(pow(reflection_spec, envGammaCorrection), brightnessDotValue);
	reflection_spec *= brightness * min(Frag.Material.Specular + 1, Frag.Material.Specular * Env_Specular(NdV, Frag.Material.Roughness));
	specular *= Frag.Material.Specular * BRDF.x + mix(vec3(1), Frag.Material.Specular, Frag.Material.Metallic) * BRDF.y;
	specular += reflection_spec;
	diffuse *= Frag.Material.Albedo.rgb * (1 - Frag.Material.Metallic);
	Frag.Material.Albedo.a += dot(specular, specular);
	Frag.Material.Albedo.a = min(1, Frag.Material.Albedo.a);

	float	mappedAlpha = map(Frag.Material.Albedo.a, 0, 1, 0.5, 1);
	Back_Color = mix(Back_Color, Back_Color * Frag.Material.Albedo.rgb, mappedAlpha);
	Back_Bright = mix(Back_Bright, Back_Bright * Frag.Material.Albedo.rgb, mappedAlpha);

	out_Color.rgb = specular + diffuse + reflection + Frag.Material.Emitting;
	out_Color.rgb = mix(Back_Color, out_Color.rgb, Frag.Material.Albedo.a);
	out_Emitting.rgb = max(vec3(0), out_Color.rgb - 1) + Frag.Material.Emitting;
	out_Emitting.rgb = mix(Back_Bright, out_Emitting.rgb, Frag.Material.Albedo.a);
}
