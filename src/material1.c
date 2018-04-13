/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/02 16:16:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:36:03 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

static inline void	material_assign_shader1(t_material *m, int si)
{
	m->shader_in[0] = shader_get_uniform_index(si, "in_Texture_Albedo");
	m->shader_in[1] = shader_get_uniform_index(si, "in_Use_Texture_Albedo");
	m->shader_in[2] = shader_get_uniform_index(si, "in_Texture_Specular");
	m->shader_in[3] = shader_get_uniform_index(si, "in_Use_Texture_Specular");
	m->shader_in[4] = shader_get_uniform_index(si, "in_Texture_Roughness");
	m->shader_in[5] = shader_get_uniform_index(si, "in_Use_Texture_Roughness");
	m->shader_in[6] = shader_get_uniform_index(si, "in_Texture_Metallic");
	m->shader_in[7] = shader_get_uniform_index(si, "in_Use_Texture_Metallic");
	m->shader_in[8] = shader_get_uniform_index(si, "in_Texture_Emitting");
	m->shader_in[9] = shader_get_uniform_index(si, "in_Use_Texture_Emitting");
	m->shader_in[10] = shader_get_uniform_index(si, "in_Texture_Normal");
	m->shader_in[11] = shader_get_uniform_index(si, "in_Use_Texture_Normal");
	m->shader_in[12] = shader_get_uniform_index(si, "in_Texture_Height");
	m->shader_in[13] = shader_get_uniform_index(si, "in_Use_Texture_Height");
	m->shader_in[14] = shader_get_uniform_index(si, "in_Texture_AO");
	m->shader_in[15] = shader_get_uniform_index(si, "in_Use_Texture_AO");
	m->shader_in[16] = shader_get_uniform_index(si, "in_Albedo");
	m->shader_in[17] = shader_get_uniform_index(si, "in_Specular");
	m->shader_in[18] = shader_get_uniform_index(si, "in_Emitting");
	m->shader_in[19] = shader_get_uniform_index(si, "in_Roughness");
	m->shader_in[20] = shader_get_uniform_index(si, "in_Metallic");
	m->shader_in[21] = shader_get_uniform_index(si, "in_Alpha");
	m->shader_in[22] = shader_get_uniform_index(si, "in_Parallax");
}

void				material_assign_shader(int material_index, int si)
{
	t_material	*m;

	m = ezarray_get_index(engine_get()->materials, material_index);
	if (!m)
		return ;
	m->shader_index = si;
	material_assign_shader1(m, si);
	m->shader_in[23] = shader_get_uniform_index(si, "in_Texture_Env");
	m->shader_in[24] = shader_get_uniform_index(si, "in_Texture_Env_Spec");
	m->shader_in[25] = shader_get_uniform_index(si, "in_Texture_BRDF");
	m->shader_in[26] = shader_get_uniform_index(si, "in_Texture_Shadow");
	m->shader_in[27] = shader_get_uniform_index(si, "in_UVMax");
	m->shader_in[28] = shader_get_uniform_index(si, "in_UVMin");
	m->shader_in[29] = shader_get_uniform_index(si, "in_CamPos");
	m->shader_in[30] = shader_get_uniform_index(si, "in_Transform");
	m->shader_in[31] = shader_get_uniform_index(si, "in_ModelMatrix");
	m->shader_in[32] = shader_get_uniform_index(si, "in_NormalMatrix");
	m->shader_in[33] = shader_get_uniform_index(si, "in_Stupidity");
	m->shader_in[34] = shader_get_uniform_index(si, "in_Texture_Stupid");
	m->shader_in[35] = shader_get_uniform_index(si, "in_ShadowTransform");
}

void				material_load_textures(int material_index)
{
	t_material	*material;
	short		*textures;
	short		i;

	material = ezarray_get_index(engine_get()->materials, material_index);
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 9)
	{
		texture_load(textures[i]);
		i++;
	}
}
