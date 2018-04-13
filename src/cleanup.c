/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/09 19:57:50 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 19:34:25 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

void	texture_destroy(void *texture)
{
	t_texture *t;

	t = texture;
	free(t->data);
	glDeleteTextures(1, &t->glid);
	destroy_ezstring(&t->name);
}

void	framebuffer_destroy(void *buffer)
{
	t_framebuffer *f;

	f = buffer;
	glDeleteFramebuffers(1, &f->id);
	destroy_ezarray(&f->color_attachements);
}

void	shader_destroy(void *shader)
{
	t_shader			*s;
	t_shadervariable	*v;
	unsigned			i;

	s = shader;
	i = 0;
	while (i < s->uniforms.length)
	{
		v = ezarray_get_index(s->uniforms, i);
		destroy_ezstring(&v->name);
		i++;
	}
	destroy_ezarray(&s->uniforms);
	i = 0;
	while (i < s->attributes.length)
	{
		v = ezarray_get_index(s->attributes, i);
		destroy_ezstring(&v->name);
		i++;
	}
	destroy_ezarray(&s->attributes);
	destroy_ezstring(&s->name);
}

void	mesh_destroy(void *mesh)
{
	t_mesh		*m;
	t_vgroup	*vg;
	unsigned	i;

	i = 0;
	m = mesh;
	while (i < m->vgroups.length)
	{
		vg = ezarray_get_index(m->vgroups, i);
		destroy_ezarray(&vg->v);
		destroy_ezarray(&vg->vn);
		destroy_ezarray(&vg->vt);
		i++;
	}
	destroy_ezarray(&m->vgroups);
}

void	cleanup(void)
{
	unsigned	i;
	t_material	*m;

	i = 0;
	destroy_ezarray(&engine_get()->cameras);
	ezforeach(engine_get()->shaders, shader_destroy);
	destroy_ezarray(&engine_get()->shaders);
	ezforeach(engine_get()->textures, texture_destroy);
	destroy_ezarray(&engine_get()->textures);
	destroy_ezarray(&engine_get()->textures_env);
	while (i < engine_get()->materials.length)
	{
		m = ezarray_get_index(engine_get()->materials, i);
		destroy_ezstring(&m->name);
		i++;
	}
	destroy_ezarray(&engine_get()->materials);
	ezforeach(engine_get()->meshes, mesh_destroy);
	destroy_ezarray(&engine_get()->meshes);
	destroy_ezarray(&engine_get()->transforms);
	destroy_ezarray(&engine_get()->lights);
	ezforeach(engine_get()->framebuffers, framebuffer_destroy);
	destroy_ezarray(&engine_get()->framebuffers);
	free(engine_get()->exec_path);
}
