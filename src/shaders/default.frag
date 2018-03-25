#version 410
#define M_PI 3.1415926535897932384626433832795

uniform vec3		in_CamPos;

uniform vec2		in_UVMax;
uniform vec2		in_UVMin;

uniform vec3		in_Albedo;
uniform vec3		in_Specular;
uniform vec3		in_Emitting;
uniform mediump float		in_Roughness;
uniform mediump float		in_Metallic;
uniform mediump float		in_Alpha;
uniform mediump float		in_Parallax;
uniform mediump float		in_Stupidity;

uniform sampler2D	in_Texture_Albedo;
uniform bool		in_Use_Texture_Albedo = false;
uniform sampler2D	in_Texture_Specular;
uniform bool		in_Use_Texture_Specular = false;
uniform sampler2D	in_Texture_Roughness;
uniform bool		in_Use_Texture_Roughness = false;
uniform sampler2D	in_Texture_Metallic;
uniform bool		in_Use_Texture_Metallic = false;
uniform sampler2D	in_Texture_Emitting;
uniform bool		in_Use_Texture_Normal = false;
uniform sampler2D	in_Texture_Normal;
uniform bool		in_Use_Texture_Height = false;
uniform sampler2D	in_Texture_Height;
uniform sampler2D	in_Texture_AO;

uniform sampler2DShadow	in_Texture_Shadow;
uniform samplerCube	in_Texture_Env;
uniform samplerCube	in_Texture_Env_Spec;
uniform sampler2D	in_Texture_BRDF;
uniform sampler2D	in_Texture_Stupid;

uniform vec3		in_LightDirection = normalize(vec3(-1, 1, 0));

in vec3			frag_ShadowPosition;
in vec3			frag_WorldNormal;
in vec3			frag_WorldPosition;
in vec3			frag_ModelNormal;
in vec3			frag_ModelPosition;
in vec2			frag_Texcoord;
in vec3			frag_Light_Color;


layout(location = 0) out vec4		out_Color;
layout(location = 1) out vec4		out_Bright;
layout(location = 2) out vec4		out_Normal;
layout(location = 3) out vec4		out_Position;

mediump float	GGX_Geometry(in mediump float NdV, in mediump float NdL, in mediump float alpha)
{
	mediump float	alpha2 = alpha * alpha;
	return (2 * NdV) / (NdV + sqrt(alpha2 + (1 - alpha2) * (NdV * NdV)));
}

mediump float	GGX_Distribution(in mediump float NdH, in mediump float alpha)
{
	mediump float den = (NdH * NdH) * (alpha - 1) + 1;
	return (alpha / (M_PI * den * den));
}

mediump float	Specular(in mediump float NdL, in mediump float NdV, in mediump float NdH, in mediump float roughness)
{
	mediump float	alpha = roughness * roughness;
	mediump float	D = GGX_Distribution(NdH, alpha);
	mediump float	G = GGX_Geometry(NdV, NdL, alpha);
	return (max(D * G, 0));
}


vec3	Fresnel(in mediump float factor, in vec3 F0, in mediump float roughness)
{
	return ((max(vec3(1 - roughness), F0) - F0) * pow(max(0, 1 - factor), 5) + F0);
}

vec2	Parallax_Mapping(in vec3 tbnV, in vec2 T, out mediump float parallaxHeight)
{
	const mediump float minLayers = 64;
	const mediump float maxLayers = 128;
	mediump float numLayers = mix(maxLayers, minLayers, abs(tbnV.z));
	int	tries = int(numLayers);
	mediump float layerHeight = 1.0 / numLayers;
	mediump float curLayerHeight = 0;
	vec2 dtex = in_Parallax * tbnV.xy / tbnV.z / numLayers;
	vec2 currentTextureCoords = T;
	mediump float heightFromTexture = 1 - texture(in_Texture_Height, currentTextureCoords).r;
	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		curLayerHeight += layerHeight; 
		currentTextureCoords -= dtex;
		heightFromTexture = 1 - texture(in_Texture_Height, currentTextureCoords).r;
	}
	vec2 prevTCoords = currentTextureCoords + dtex;
	mediump float nextH	= heightFromTexture - curLayerHeight;
	mediump float prevH	= 1 - texture(in_Texture_Height, prevTCoords).r
	- curLayerHeight + layerHeight;
	mediump float weight = nextH / (nextH - prevH);
	vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0 - weight);
	parallaxHeight = (curLayerHeight + prevH * weight + nextH * (1.0 - weight));
	parallaxHeight *= in_Parallax;
	parallaxHeight = isnan(parallaxHeight) ? 0 : parallaxHeight;
	return finalTexCoords;
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

mediump float	CustomLambertianDiffuse(mediump float NdL, mediump float roughness)
{
	return pow(NdL, 0.5 * (1 - roughness) + 0.5);
}

void	main()
{
	vec3	worldPosition = frag_WorldPosition;
	vec3	worldNormal = normalize(frag_WorldNormal);
	mat3x3	tbn;
	vec2	vt = frag_Texcoord;
	vec4	albedo = vec4(in_Albedo, in_Alpha);

	tbn = tbn_matrix(frag_WorldPosition, frag_WorldNormal, frag_Texcoord);
	if (in_Use_Texture_Height)
	{
		mediump float ph;
		vt = Parallax_Mapping(tbn * normalize(in_CamPos - worldPosition), vt, ph);
		worldPosition = worldPosition - (worldNormal * ph);
	}
	vec4	albedo_sample = texture(in_Texture_Albedo, vt);
	vec3	emitting = texture(in_Texture_Emitting, vt).rgb + in_Emitting;
	vec3	normal_sample = texture(in_Texture_Normal, vt).xyz * 2 - 1;
	vec3	specular_sample = texture(in_Texture_Specular, vt).xyz;
	mediump float	roughness_sample = texture(in_Texture_Roughness, vt).r;
	mediump float	metallic_sample = texture(in_Texture_Metallic, vt).r;
	mediump float	ao = 1 - texture(in_Texture_AO, vt).r;
	vec4	stupid_sample = texture(in_Texture_Stupid, vt);
	vec3	light_Color = texture(in_Texture_Shadow, vec3(frag_ShadowPosition.xy, frag_ShadowPosition.z * 0.995)) * frag_Light_Color;

	if (in_Use_Texture_Albedo)
	{
		albedo.rgb = albedo_sample.rgb;
		albedo.a *= albedo_sample.a;
	}
	if (in_Use_Texture_Normal)
	{
		vec3	new_normal = normal_sample * tbn;
		if (dot(new_normal, new_normal) > 0)
			worldNormal = normalize(new_normal);
	}
	if (albedo.a <= 0.05
	|| vt.x > in_UVMax.x || vt.y > in_UVMax.y
	|| vt.x < in_UVMin.x || vt.y < in_UVMin.y)
		discard;
	mediump float	roughness = clamp(in_Use_Texture_Roughness ? roughness_sample : in_Roughness, 0.05f, 1.f);
	mediump float	metallic = clamp(in_Use_Texture_Metallic ? metallic_sample : in_Metallic, 0.f, 1.f);
	vec3	F0 = mix(in_Use_Texture_Specular ? specular_sample : in_Specular, albedo.rgb, metallic);
	vec3	V = normalize(in_CamPos - worldPosition);
	vec3	H = normalize(in_LightDirection + V);
	vec3	R = reflect(V, worldNormal);
	mediump float	NdH = max(0, dot(worldNormal, H));
	mediump float	NdL = max(0, dot(worldNormal, in_LightDirection));
	mediump float	NdV = max(0, dot(worldNormal, V));
	mediump float	LdH = max(0, dot(worldNormal, H));

	vec3	fresnel = Fresnel(NdV, F0, roughness);
	vec2	BRDF = texture(in_Texture_BRDF, vec2(NdV, roughness)).rg;

	vec3	diffuse = (textureLod(in_Texture_Env, -worldNormal, roughness + 9).rgb
			+ textureLod(in_Texture_Env_Spec, -worldNormal, roughness * 4.f).rgb) * ao;
	vec3	reflection = textureLod(in_Texture_Env, R, roughness * 11.f).rgb * fresnel;
	vec3	reflection_spec = pow(textureLod(in_Texture_Env, R, roughness * 11.f + 3.5).rgb, vec3(4));
	vec3	specular = textureLod(in_Texture_Env_Spec, R, roughness * 5.f).rgb;
	
	reflection_spec *= (1 - fresnel) * (1 - roughness) * dot(reflection_spec, vec3(0.299, 0.587, 0.114));
	specular *= fresnel * BRDF.x + mix(vec3(1), fresnel, metallic) * BRDF.y;
	specular += reflection_spec;

	fresnel = Fresnel(LdH, F0, roughness);
	specular += light_Color * min(vec3(5), fresnel * Specular(NdL, NdV, NdH, roughness));
	diffuse += light_Color * CustomLambertianDiffuse(NdL, roughness);
	diffuse *= albedo.rgb * (1 - metallic);

	albedo.a += dot(specular, specular);
	albedo.a = min(1, albedo.a);
	out_Color.rgb = emitting + specular + diffuse + reflection;
	out_Color.a = albedo.a;
	out_Color = mix(out_Color, stupid_sample, in_Stupidity);
	out_Bright = vec4(max(vec3(0), out_Color.rgb - 1) + emitting, albedo.a);
	out_Normal = vec4(worldNormal, 1);
	out_Position = vec4(worldPosition, 1);
}
