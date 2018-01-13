#version 330 core
#define M_PI 3.1415926535897932384626433832795

uniform vec3	in_CamPos;
uniform vec3	in_Albedo;
uniform vec2	in_UVScale;
uniform float	in_Specular;
uniform float	in_Roughness;
uniform float	in_Metallic;
uniform float	in_Refraction;
uniform float	in_Alpha;
uniform float	in_Parallax;
in vec3			frag_Normal;
in vec3			frag_Position;
out vec4		out_Color;

/*float	attenuation(t_light l, VEC3 position)
{
	float	a;

	if (l.type == DIRECTIONAL)
		return (l.power);
	a = 1 / pow(fmax(vec3_distance(l.position, position) - l.falloff, 0)
		/ l.falloff + 1, 2) * l.power;
	return ((a - l.attenuation) / (1 - l.attenuation));
}*/

float	f0_term(float r)
{
	return (pow(abs((1.0 - r) / (1.0 + r)), 2));
}

float	g1v(float dotnv, float k)
{
	return (1.0f / (dotnv * (1.0f - k) + k));
}

float				ggx_specular(vec3 normal, vec3 viewdir,
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
	d[0] = clamp(dot(normal, lightdir), 0, 1);
	d[1] = clamp(dot(normal, h), 0, 1);
	a[1] = a[0] * a[0];
	k = a[0] / 2.0f;
	return (d[0] * (a[1] / (M_PI * pow(d[1] * d[1] * (a[1] - 1.0) + 1.0f, 2))) *
	(f0 + (1.0 - f0) * (pow(1.0f - clamp(dot(lightdir, h), 0, 1), 5)))
	* g1v(d[0], k) * g1v(clamp(dot(normal, viewdir), 0, 1), k));
}

float	trowbridge_reitz_specular(vec3 normal, vec3 viewdir, vec3 ldir)
{
	float	r2;
	float	r21;
	vec3	m;

	r2 = in_Roughness * in_Roughness;
	r21 = r2 - 1;
	m = normalize(((ldir + viewdir)));
	return (r2 / (M_PI * pow(pow(dot(normal, m), 2) * r21 + 1, 2)));
}

float	oren_nayar_diffuse(vec3 normal, vec3 viewdir, vec3 ldir)
{
	float	ndl;
	float	nde;
	float	lde;
	float	r2;

	ndl = dot(ldir, normal);
	nde = dot(normal, viewdir);
	lde = dot(ldir, viewdir) - ndl * nde;
	r2 = in_Roughness * in_Roughness;
	return (clamp(
		max(0.0, ndl) * ((1.0 + r2 * (1 / (r2 + 0.13) + 0.5 / (r2 + 0.33))) +
		(0.45 * r2 / (r2 + 0.09)) * lde / mix(1.0, max(ndl, nde),
		step(0.0, lde))) / M_PI, 0, 1));
}

void main()
{
	vec3	light_Pos = vec3(5, 5, 0);
	vec3	light_Color = vec3(1, 1, 1);
	float	light_Power = 5;

	float	light_attenuation = light_Power * 1.f / (1 + distance(light_Pos, frag_Position));

	float	f_Albedo = oren_nayar_diffuse(frag_Normal, vec3(in_CamPos - frag_Position), vec3(light_Pos - frag_Position));
	float	f_Specular = ggx_specular(frag_Normal, vec3(in_CamPos - frag_Position), vec3(light_Pos - frag_Position));
	vec3	c_Albedo = in_Albedo * f_Albedo;
	vec3	c_Specular = (light_Color + in_Albedo) * f_Specular;
	out_Color = vec4((c_Albedo + c_Specular) * light_attenuation, 1);
	//out_Color = vec4(frag_Normal, 1);
  //out_Color = vec4(frag_Normal, 1);
  //out_Color = vec4(1, 0, 0, 1);
}
