#version 410
#pragma optionNV (unroll all)
#define	KERNEL_SIZE 64
#define M_PI 3.1415926535897932384626433832795

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

/* uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Bright;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position; */

uniform sampler2D	in_Texture_Albedo;
uniform sampler2D	in_Texture_Fresnel;
uniform sampler2D	in_Texture_Emitting;
uniform sampler2D	in_Texture_Material_Values;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_Normal;
uniform sampler2D	in_Texture_Position;
uniform sampler2D	in_Texture_Depth;
uniform vec3		in_CamPos;

uniform t_Environment	Environment;

in vec2				frag_UV;
in vec3				frag_Cube_UV;
in float			frag_CenterDepth;

out vec4			out_Color;

uniform vec2 poissonDisk[] = vec2[KERNEL_SIZE](
	vec2(-0.613392, 0.617481),
	vec2(0.170019, -0.040254),
	vec2(-0.299417, 0.791925),
	vec2(0.645680, 0.493210),
	vec2(-0.651784, 0.717887),
	vec2(0.421003, 0.027070),
	vec2(-0.817194, -0.271096),
	vec2(-0.705374, -0.668203),
	vec2(0.977050, -0.108615),
	vec2(0.063326, 0.142369),
	vec2(0.203528, 0.214331),
	vec2(-0.667531, 0.326090),
	vec2(-0.098422, -0.295755),
	vec2(-0.885922, 0.215369),
	vec2(0.566637, 0.605213),
	vec2(0.039766, -0.396100),
	vec2(0.751946, 0.453352),
	vec2(0.078707, -0.715323),
	vec2(-0.075838, -0.529344),
	vec2(0.724479, -0.580798),
	vec2(0.222999, -0.215125),
	vec2(-0.467574, -0.405438),
	vec2(-0.248268, -0.814753),
	vec2(0.354411, -0.887570),
	vec2(0.175817, 0.382366),
	vec2(0.487472, -0.063082),
	vec2(-0.084078, 0.898312),
	vec2(0.488876, -0.783441),
	vec2(0.470016, 0.217933),
	vec2(-0.696890, -0.549791),
	vec2(-0.149693, 0.605762),
	vec2(0.034211, 0.979980),
	vec2(0.503098, -0.308878),
	vec2(-0.016205, -0.872921),
	vec2(0.385784, -0.393902),
	vec2(-0.146886, -0.859249),
	vec2(0.643361, 0.164098),
	vec2(0.634388, -0.049471),
	vec2(-0.688894, 0.007843),
	vec2(0.464034, -0.188818),
	vec2(-0.440840, 0.137486),
	vec2(0.364483, 0.511704),
	vec2(0.034028, 0.325968),
	vec2(0.099094, -0.308023),
	vec2(0.693960, -0.366253),
	vec2(0.678884, -0.204688),
	vec2(0.001801, 0.780328),
	vec2(0.145177, -0.898984),
	vec2(0.062655, -0.611866),
	vec2(0.315226, -0.604297),
	vec2(-0.780145, 0.486251),
	vec2(-0.371868, 0.882138),
	vec2(0.200476, 0.494430),
	vec2(-0.494552, -0.711051),
	vec2(0.612476, 0.705252),
	vec2(-0.578845, -0.768792),
	vec2(-0.772454, -0.090976),
	vec2(0.504440, 0.372295),
	vec2(0.155736, 0.065157),
	vec2(0.391522, 0.849605),
	vec2(-0.620106, -0.328104),
	vec2(0.789239, -0.419965),
	vec2(-0.545396, 0.538133),
	vec2(-0.178564, -0.596057));

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
	float	Depth = texture(in_Texture_Depth, frag_UV).r;
	vec3	EnvDiffuse = texture(Environment.Diffuse, frag_Cube_UV).rgb;
	if (Depth == 1) {
		out_Color.rgb = EnvDiffuse;
		return ;
	}
	vec3	EnvIrradiance = texture(Environment.Diffuse, frag_Cube_UV).rgb;
	vec4	Albedo = texture(in_Texture_Albedo, frag_UV);
	vec3	Fresnel = texture(in_Texture_Fresnel, frag_UV).rgb;
	vec4	Emitting = texture(in_Texture_Emitting, frag_UV);
	vec4	Material_Values = texture(in_Texture_Material_Values, frag_UV);
	float	Roughness = Material_Values.x;
	float	Metallic = Material_Values.y;
	float	AO = Material_Values.z;
	vec4	BRDF = texture(in_Texture_BRDF, frag_UV);
	vec3	Normal = normalize(texture(in_Texture_Normal, frag_UV).xyz);
	vec3	Position = texture(in_Texture_Position, frag_UV).xyz;

	vec2	sampledist = textureSize(in_Texture_Position, 0) / 1024.f * 25.f;
	vec2	sampleOffset = sampledist / textureSize(in_Texture_Position, 0);
	float	occlusion = 0.f;
	for (int i = 0; i < KERNEL_SIZE; ++i)
	{
		vec2	sampleUV = frag_UV + poissonDisk[i] * sampleOffset;
		vec3	samplePosition = texture(in_Texture_Position, sampleUV).xyz;
		if (texture(in_Texture_Depth, sampleUV).r < Depth)
		{
			vec3	V = samplePosition - Position;
			float	D = length(V);
			float	bias = 0.0025;
			float	factor = max(0, dot(Normal, normalize(V)));
			float	angle = max(0, factor - bias);
			occlusion += (angle * (1.f / (1.f + D)));
		}
	}
	occlusion /= float(KERNEL_SIZE);
	occlusion = 1 - occlusion;
	AO += occlusion;

	vec3	diffuse = AO * (textureLod(Environment.Diffuse, -Normal, Roughness + 9).rgb
			+ textureLod(Environment.Irradiance, -Normal, Roughness * 4.f).rgb);
	vec3	V = normalize(in_CamPos - Position);
	float	NdV = max(0, dot(Normal, V));
	vec3	R = reflect(V, Normal);
	vec3	reflection = textureLod(Environment.Diffuse, R, Roughness * 12.f).rgb * Fresnel;
	vec3	specular = textureLod(Environment.Irradiance, R, Roughness * 10.f).rgb;
	vec3	reflection_spec = pow(textureLod(Environment.Diffuse, R, Roughness * 10.f + 3.5).rgb, vec3(2.2));

	float	brightness = dot(reflection_spec, vec3(0.299, 0.587, 0.114));
	reflection_spec *= brightness * min(Fresnel + 1, Fresnel * Env_Specular(NdV, Roughness));
	specular *= Fresnel * BRDF.x + mix(vec3(1), Fresnel, Metallic) * BRDF.y;
	specular += reflection_spec;
	diffuse *= Albedo.rgb * (1 - Metallic);
	Albedo.a += dot(specular, specular);
	Albedo.a = min(1, Albedo.a);

	out_Color.rgb = Emitting.rgb + specular + diffuse + reflection;
	//out_Color.rgb = vec3(diffuse);
}
