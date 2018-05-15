#version 410
#define M_PI 3.1415926535897932384626433832795

uniform lowp vec3		in_CamPos;

uniform lowp vec3		in_Albedo;
uniform lowp vec3		in_Emitting;
uniform lowp float		in_Alpha;

uniform sampler2D	in_Texture_Albedo;
uniform bool		in_Use_Texture_Albedo = false;
uniform sampler2D	in_Texture_Emitting;
uniform bool		in_Use_Texture_Normal = false;
uniform sampler2D	in_Texture_Normal;

uniform samplerCube	in_Texture_Env;
uniform samplerCube	in_Texture_Env_Spec;
uniform sampler2D	in_Texture_BRDF;

in vec3			frag_WorldPosition;
in lowp vec3	frag_WorldNormal;
in lowp vec2	frag_Texcoord;

layout(location = 0) out lowp vec4	out_Color;
layout(location = 1) out lowp vec4	out_Bright;
layout(location = 2) out lowp vec4	out_Normal;
layout(location = 3) out vec4		out_Position;

lowp vec3	Fresnel(in lowp float factor, in lowp vec3 F0, in lowp float roughness)
{
	return ((max(vec3(1 - roughness), F0)) * pow(max(0, 1 - factor), 5) + F0);
}

mat3x3	tbn_matrix(in vec3 position, in vec3 normal, in vec2 texcoord)
{
	vec3 Q1 = dFdx(position);
	vec3 Q2 = dFdy(position);
	vec2 st1 = dFdx(texcoord);
	vec2 st2 = dFdy(texcoord);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, normal)));
}

lowp float	Env_Specular(in lowp float NdV, in lowp float roughness)
{
	lowp float	alpha = roughness * roughness;
	lowp float	den = (alpha - 1) + 1;
	lowp float	D = (alpha / (M_PI * den * den));
	lowp float	alpha2 = alpha * alpha;
	lowp float	G = (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
	return (max(D * G, 0));
}

void	main()
{
	vec3	worldPosition = frag_WorldPosition;
	lowp vec3	worldNormal = normalize(frag_WorldNormal);
	lowp vec4	albedo = vec4(in_Albedo, in_Alpha);
	lowp vec4	albedo_sample = texture(in_Texture_Albedo, frag_Texcoord);
	lowp vec3	emitting = texture(in_Texture_Emitting, frag_Texcoord).rgb + in_Emitting;
	lowp vec3	normal_sample = texture(in_Texture_Normal, frag_Texcoord).xyz * 2 - 1;

	if (in_Use_Texture_Albedo)
	{
		albedo.rgb = albedo_sample.rgb;
		albedo.a *= albedo_sample.a;
	}
	if (in_Use_Texture_Normal)
	{
		lowp vec3	new_normal = normal_sample * tbn_matrix(frag_WorldPosition, frag_WorldNormal, frag_Texcoord);
		if (dot(new_normal, new_normal) > 0)
			worldNormal = normalize(new_normal);
	}
	if (albedo.a <= 0.05)
		discard;
	lowp vec3	V = normalize(in_CamPos - worldPosition);
	lowp vec3	R = reflect(V, worldNormal);
	lowp float	NdV = max(0, dot(worldNormal, V));

	lowp vec3	fresnel = Fresnel(NdV, vec3(0.04), 0.5);
	lowp vec2	BRDF = texture(in_Texture_BRDF, vec2(NdV, 0.5)).rg;
	lowp vec3	diffuse = (textureLod(in_Texture_Env, -worldNormal, 0.5 + 9).rgb
			+ textureLod(in_Texture_Env_Spec, -worldNormal, 0.5 * 4.f).rgb);
	lowp vec3	reflection = textureLod(in_Texture_Env, R, 0.5 * 12.f).rgb * fresnel;
	lowp vec3	specular = textureLod(in_Texture_Env_Spec, R, 0.5 * 10.f).rgb;
	lowp vec3	reflection_spec = pow(textureLod(in_Texture_Env, R, 0.5 * 10.f + 3.5).rgb, vec3(4));
	lowp float	brightness = dot(reflection_spec, vec3(0.299, 0.587, 0.114));
	reflection_spec *= brightness * min(fresnel + 1, fresnel * Env_Specular(NdV, 0.5));
	specular *= fresnel * BRDF.x + BRDF.y;
	specular += reflection_spec;
	diffuse *= albedo.rgb;

	albedo.a += dot(specular, specular);
	albedo.a = min(1, albedo.a);
	out_Color.rgb = emitting + specular + diffuse + reflection;
	out_Color.a = albedo.a;
	out_Bright = vec4(max(vec3(0), out_Color.rgb - 1) + emitting, albedo.a);
	out_Normal = vec4(worldNormal, 1);
	out_Position = vec4(worldPosition, 1);
}
