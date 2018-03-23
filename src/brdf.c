/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   brdf.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/21 20:58:41 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/23 01:03:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float a = roughness * roughness;
	float k = (a * a) / 2.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(VEC3 N, VEC3 V, VEC3 L, float roughness)
{
	float NdotV = fmax(vec3_dot(N, V), 0.0);
	float NdotL = fmax(vec3_dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float GGX(float NoV, float roughness)
{
	float r2 = pow(roughness, 2);
	return NoV*2 / (NoV + sqrt((NoV*NoV) * (1.0f - r2) + r2));
}

float geometryForLut(float roughness, float NoL)
{
	return GGX(NoL, roughness * roughness);
}

float visibilityForLut(float roughness, float NoV)
{
	return GGX(NoV, roughness * roughness);
}

VEC2 sumLut(VEC2 current, float G, float V, float F, float VoH, float NoH, float NoV)
{
	float G_Vis;

	G = G * V;
	G_Vis = G * VoH / (NoH * NoV);
	current.x += (1.0 - F) * G_Vis;
	current.y += F * G_Vis;
	return (current);
}

VEC3 ImportanceSampleGGX(VEC2 Xi, VEC3 N, float roughness)
{
	VEC3	H;
	float	a;
	float	phi;
	float	cosTheta;
	float	sinTheta;
	VEC3	up;
	VEC3	tangent;
	VEC3	bitangent;
	VEC3	sampleVec;

	a = roughness*roughness;
	phi = 2.0 * 3.14 * Xi.x;
	cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	up = abs(N.z) < 0.999 ? new_vec3(0.0, 0.0, 1.0) : new_vec3(1.0, 0.0, 0.0);
	tangent = vec3_normalize(vec3_cross(up, N));
	bitangent = vec3_cross(N, tangent);
	
	sampleVec = vec3_scale(tangent, H.x);
	sampleVec = vec3_add(sampleVec, vec3_scale(bitangent, H.y));
	sampleVec = vec3_add(sampleVec, vec3_scale(N, H.z));
	return vec3_normalize(sampleVec);
}  

float RadicalInverse_VdC(unsigned bits) 
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return (float)(bits) * 2.3283064365386963e-10; // / 0x100000000
}

VEC2 Hammersley(unsigned i, unsigned N)
{
	return new_vec2((float)(i)/(float)(N), RadicalInverse_VdC(i));
}

VEC2 IntegrateBRDF(float NdotV, float roughness)
{
	VEC3 V;
	V.x = sqrt(1.0 - NdotV*NdotV);
	V.y = 0.0;
	V.z = NdotV;

	float A = 0.0;
	float B = 0.0;
	float NdotL;
	float NdotH;
	float VdotH;
	float G = 0;
	float G_Vis = 0;
	float Fc = 0;

	VEC3 N = new_vec3(0.0, 0.0, 1.0);
	VEC3 H;
	VEC3 L;

	const unsigned SAMPLE_COUNT = 1024u;
	G_Vis = GGX(NdotV, roughness * roughness);
	VEC2	sum = new_vec2(0, 0);
	for(unsigned i = 0u; i < SAMPLE_COUNT; ++i)
	{
		VEC2 Xi = Hammersley(i, SAMPLE_COUNT);
		H  = ImportanceSampleGGX(Xi, N, roughness);
		L  = vec3_normalize(vec3_sub(vec3_scale(H, 2.0 * vec3_dot(V, H)), V));

		NdotL = fmax(L.z, 0.0);
		NdotH = fmax(H.z, 0.0);
		VdotH = fmax(vec3_dot(V, H), 0.0);

		if(NdotL > 0.0)
		{
			G = GGX(NdotL, roughness * roughness);
			Fc = pow(1.0 - VdotH, 5.0);
			sum = sumLut(sum, G, G_Vis, Fc, VdotH, NdotH, NdotV);
		}
	}
	A = sum.x / (float)(SAMPLE_COUNT);
	B = sum.y / (float)(SAMPLE_COUNT);
	return new_vec2(A, B);
}

int texture_generate_brdf()
{
	int			texture;
	t_texture	*t;

	texture = texture_create(new_vec2(256, 256), GL_TEXTURE_2D, GL_BGR, GL_RGB);
	t = ezarray_get_index(engine_get()->textures, texture);
	t->data = ft_memalloc(sizeof(UCHAR) * 3 * t->size.x * t->size.y);
	t->bpp = 24;
	t->name = new_ezstring("BRDF");
	texture_set_parameters(texture, 2,
		(GLenum[2]){GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[2]){GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE});
	int x = 0, y;
	while (x <= t->size.x)
	{
		y = 0;
		while (y <= t->size.y)
		{
			VEC2    uv = new_vec2(x / t->size.x, y / t->size.y);
			VEC2    brdf = IntegrateBRDF(uv.x, uv.y);
			texture_set_pixel(texture, new_vec2(1 - uv.x, uv.y), new_vec4(0, brdf.y, brdf.x, 1));
			y++;
		}
		x++;
	}
	return (texture);
}