/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/09 19:57:50 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/09 20:34:16 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

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

void	material_destroy(void *material)
{
	t_material *m;

	m = material;
	destroy_ezstring(&m->name);
}

void	shader_variable_destroy(void *variable)
{
	t_shadervariable *v;

	v = variable;
	destroy_ezstring(&v->name);
}

void	shader_destroy(void *shader)
{
	t_shader *s;

	s = shader;
	ezforeach(s->uniforms, shader_variable_destroy);
	destroy_ezarray(&s->uniforms);
	ezforeach(s->attributes, shader_variable_destroy);
	destroy_ezarray(&s->attributes);
}

void	vgroup_destroy(void *vgroup)
{
	t_vgroup *vg;

	vg = vgroup;
	destroy_ezarray(&vg->v);
	destroy_ezarray(&vg->vn);
	destroy_ezarray(&vg->vt);
}

void	mesh_destroy(void *mesh)
{
	t_mesh *m;

	m = mesh;
	ezforeach(m->vgroups, vgroup_destroy);
	destroy_ezarray(&m->vgroups);
}

void	cleanup()
{
	destroy_ezarray(&engine_get()->cameras);
	ezforeach(engine_get()->shaders, shader_destroy);
	destroy_ezarray(&engine_get()->shaders);
	ezforeach(engine_get()->textures, texture_destroy);
	destroy_ezarray(&engine_get()->textures);
	destroy_ezarray(&engine_get()->textures_env);
	ezforeach(engine_get()->materials, material_destroy);
	destroy_ezarray(&engine_get()->materials);
	ezforeach(engine_get()->meshes, mesh_destroy);
	destroy_ezarray(&engine_get()->meshes);
	destroy_ezarray(&engine_get()->transforms);
	destroy_ezarray(&engine_get()->lights);
	ezforeach(engine_get()->framebuffers, framebuffer_destroy);
	destroy_ezarray(&engine_get()->framebuffers);
}