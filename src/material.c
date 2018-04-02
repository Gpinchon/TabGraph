/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/02 21:50:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int		material_create(char *name)
{
	t_material	mtl;

	ft_memset(&mtl, 0, sizeof(t_material));
	ft_memset(&mtl.data, -1, sizeof(t_mtl));
	mtl.data.texture_stupid = texture_get_by_name("stupid");
	mtl.data.stupidity = 0;
	mtl.data.alpha = 1;
	mtl.data.parallax = 0.01;
	mtl.data.albedo = new_vec3(0.5, 0.5, 0.5);
	mtl.data.emitting = new_vec3(0, 0, 0);
	mtl.data.metallic = 0;
	mtl.data.roughness = 0.5;
	mtl.data.specular = new_vec3(0.04, 0.04, 0.04);
	mtl.name = new_ezstring(name);
	mtl.id = hash((unsigned char *)mtl.name.tostring);
	ezarray_push(&engine_get()->materials, &mtl);
	material_assign_shader(engine_get()->materials.length - 1,
		shader_get_by_name("default"));
	return (engine_get()->materials.length - 1);
}

int		material_get_index_by_name(char *name)
{
	int			i;
	ULL			h;
	t_material	*m;

	i = 0;
	h = hash((unsigned char*)name);
	while ((m = ezarray_get_index(engine_get()->materials, i)))
	{
		if (h == m->id)
			return (i);
		i++;
	}
	return (-1);
}

int		material_get_index_by_id(ULL h)
{
	int			i;
	t_material	*m;

	i = 0;
	while ((m = ezarray_get_index(engine_get()->materials, i)))
	{
		if (h == m->id)
			return (i);
		i++;
	}
	return (0);
}

void	material_bind_textures(int material_index)
{
	t_material	*m;
	short		*st;
	short		*t;
	short		i;

	m = ezarray_get_index(engine_get()->materials, material_index);
	st = &m->shader_in[0];
	t = &m->data.texture_albedo;
	i = 0;
	while (i < 8)
	{
		shader_bind_texture(m->shader_index, st[i * 2 + 0],
			t[i], GL_TEXTURE1 + i);
		shader_set_int(m->shader_index, st[i * 2 + 1], t[i] == -1 ? 0 : 1);
		i++;
	}
	shader_bind_texture(m->shader_index, m->shader_in[23],
		engine_get()->env, GL_TEXTURE11);
	shader_bind_texture(m->shader_index, m->shader_in[24],
		engine_get()->env_spec, GL_TEXTURE12);
	shader_bind_texture(m->shader_index, m->shader_in[25],
		engine_get()->brdf_lut, GL_TEXTURE13);
	shader_bind_texture(m->shader_index, m->shader_in[34],
		m->data.texture_stupid, GL_TEXTURE14);
}

void	material_set_uniforms(int material_index)
{
	t_material	*m;

	m = ezarray_get_index(engine_get()->materials, material_index);
	if (!m)
		return ;
	material_bind_textures(material_index);
	shader_set_vec3(m->shader_index, m->shader_in[16], m->data.albedo);
	shader_set_vec3(m->shader_index, m->shader_in[17], m->data.specular);
	shader_set_vec3(m->shader_index, m->shader_in[18], m->data.emitting);
	shader_set_float(m->shader_index, m->shader_in[19], m->data.roughness);
	shader_set_float(m->shader_index, m->shader_in[20], m->data.metallic);
	shader_set_float(m->shader_index, m->shader_in[21], m->data.alpha);
	shader_set_float(m->shader_index, m->shader_in[22], m->data.parallax);
	shader_set_float(m->shader_index, m->shader_in[33], m->data.stupidity);
}
