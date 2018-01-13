#version 330 core
#define M_PI 3.1415926535897932384626433832795

uniform vec3		in_CamPos;
uniform vec3		in_Albedo;
uniform vec2		in_UVScale;
uniform float		in_Specular;
uniform float		in_Roughness;
uniform float		in_Metallic;
uniform float		in_Refraction;
uniform float		in_Alpha;
uniform float		in_Parallax;
in vec3			frag_Normal;
in vec3			frag_Position;
out vec4		out_Color;

float D_GGX(in float NdH)
{
    float m = in_Roughness * in_Roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (M_PI * d * d);
}

float G_schlick(in float NdV, in float NdL)
{
    float k = in_Roughness * in_Roughness * 0.5;
    float V = NdV * (1.0 - k) + k;
    float L = NdL * (1.0 - k) + k;
    return 0.25 / (V * L);
}

vec3 S_Cooktorrance(in float NdL, in float NdV, in float NdH, in vec3 specular)
{
    float D = D_GGX(NdH);

    float G = G_schlick(NdV, NdL);

    float rim = mix(1.0 - in_Roughness * 0.9 * in_Refraction, 1.0, NdV);

    return (1.0 / rim) * specular * G * D;
}

float D_Phong(in float NdL)
{
    return ((1.0 / M_PI) * NdL);
}

vec3 fresnel_factor(in vec3 f0, in float product)
{
    return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
}

void main()
{
	vec3	light_Pos = vec3(5, 5, 0);
	vec3	light_Color = vec3(1, 1, 1);
	float	light_Power = 10;
	float	light_Attenuation = light_Power * 1.f / (1 + distance(light_Pos, frag_Position));
	light_Color *= light_Attenuation;

	vec3 L = normalize(light_Pos - frag_Position);
    vec3 V = normalize(in_CamPos - frag_Position);
    vec3 H = normalize(L + V);
	float NdL = max(0.0, dot(frag_Normal, L));
    float NdV = max(0.001, dot(frag_Normal, V));
    float NdH = max(0.001, dot(frag_Normal, H));
    float HdV = max(0.001, dot(H, V));
    float LdV = max(0.001, dot(L, V));

    vec3	c_Specular = mix(vec3(0.04), in_Albedo, in_Metallic);
    vec3	f_Fresnel = fresnel_factor(c_Specular, HdV);
    vec3	f_Specular = S_Cooktorrance(NdL, NdV, NdH, f_Fresnel) * vec3(NdL);
	vec3	f_Diffuse = (vec3(1) - f_Fresnel) * D_Phong(NdL);
	vec3	c_RefLight = f_Specular * light_Color;
	vec3	c_DifLight = f_Diffuse * light_Color;
	
	//out_Color = vec4(c_RefLight, 1);
	out_Color = vec4(c_DifLight * mix(in_Albedo, vec3(0), in_Metallic) /** c_RefLight*/, 1);
}




/*float	attenuation(t_light l, VEC3 position)
{
	float	a;

	if (l.type == DIRECTIONAL)
		return (l.power);
	a = 1 / pow(fmax(vec3_distance(l.position, position) - l.falloff, 0)
		/ l.falloff + 1, 2) * l.power;
	return ((a - l.attenuation) / (1 - l.attenuation));
}*/

/*float D_GGX(vec3 viewdir,
	vec3 lightdir)
{
	float NdH = dot(frag_Normal, normalize(viewdir + lightdir));
    float m = in_Roughness * in_Roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (M_PI * d * d);
}

float	f0_term(float r)
{
	return (pow(abs((1.0 - r) / (1.0 + r)), 2));
}

float	g1v(float dotnv, float k)
{
	return (1.0f / (dotnv * (1.0f - k) + k));
}

float				ggx_specular(vec3 viewdir,
	vec3 lightdir)
{
	float	a[2];
	float	d[2];
	float	f0;
	float	k;
	vec3	h;

	f0 = f0_term(in_Refraction);
	a[0] = in_Roughness * in_Roughness;
	h = normalize(viewdir + lightdir);
	d[0] = clamp(dot(frag_Normal, lightdir), 0, 1);
	d[1] = clamp(dot(frag_Normal, h), 0, 1);
	a[1] = a[0] * a[0];
	k = a[0] / 2.0f;
	return (d[0] * (a[1] / (M_PI * pow(d[1] * d[1] * (a[1] - 1.0) + 1.0f, 2))) *
	(f0 + (1.0 - f0) * (pow(1.0f - clamp(dot(lightdir, h), 0, 1), 5)))
	* g1v(d[0], k) * g1v(clamp(dot(frag_Normal, viewdir), 0, 1), k));
}

float	trowbridge_reitz_specular(vec3 viewdir, vec3 ldir)
{
	float	r2;
	float	r21;
	vec3	m;

	r2 = in_Roughness * in_Roughness;
	r21 = r2 - 1;
	m = normalize(((ldir + viewdir)));
	return (r2 / (M_PI * pow(pow(dot(frag_Normal, m), 2) * r21 + 1, 2)));
}

float	oren_nayar_diffuse(vec3 viewdir, vec3 ldir)
{
	float	ndl;
	float	nde;
	float	lde;
	float	r2;

	ndl = dot(ldir, frag_Normal);
	nde = dot(frag_Normal, viewdir);
	lde = dot(ldir, viewdir) - ndl * nde;
	r2 = in_Roughness * in_Roughness;
	return (clamp(
		max(0.0, ndl) * ((1.0 + r2 * (1 / (r2 + 0.13) + 0.5 / (r2 + 0.33))) +
		(0.45 * r2 / (r2 + 0.09)) * lde / mix(1.0, max(ndl, nde),
		step(0.0, lde))) / M_PI, 0, 1));
}*/
