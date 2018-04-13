/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_shadow.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 15:50:41 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:35:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

static FRUSTUM	full_scene_frustum()
{
	t_mesh		*mesh;
	t_transform	*t;
	float		value;
	int			mesh_index;
	VEC3		v[2];

	v[0] = new_vec3(1000, 1000, 1000);
	v[1] = new_vec3(-1000, -1000, -1000);
	mesh_index = 0;
	while ((mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
	{
		t = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
		VEC3	curmin, curmax;
		curmin = mat4_mult_vec3(t->transform, mesh->bounding_box.min);
		curmax = mat4_mult_vec3(t->transform, mesh->bounding_box.max);
		v[0].x = curmin.x < v[0].x ? curmin.x : v[0].x;
		v[0].y = curmin.y < v[0].y ? curmin.y : v[0].y;
		v[0].z = curmin.z < v[0].z ? curmin.z : v[0].z;
		v[1].x = curmax.x > v[1].x ? curmax.x : v[1].x;
		v[1].y = curmax.y > v[1].y ? curmax.y : v[1].y;
		v[1].z = curmax.z > v[1].z ? curmax.z : v[1].z;
		mesh_index++;
	}
	value = MAX(1.5, vec3_distance(v[0], v[1]) / 2.f);
	return (new_frustum(-value, value, -value, value));
}

static void		render_shadow_vgroup(t_vgroup *vg, int rb, MAT4 *mvp)
{
	t_material	*m;

	if (!vg)
		return ;
	m = ezarray_get_index(engine_get()->materials, vg->mtl_index);
	if (!m || m->data.alpha <= 0.5f)
		return ;
	shader_set_mat4(m->shader_index, m->shader_in[35], *mvp);
	shader_bind_texture(m->shader_index,
	m->shader_in[26], framebuffer_get_depth(rb), GL_TEXTURE20);
	shader_use(framebuffer_get_shader(rb));
	shader_bind_texture(framebuffer_get_shader(rb), shader_get_uniform_index(
		framebuffer_get_shader(rb), "in_Texture_Albedo"),
		m->data.texture_albedo, GL_TEXTURE0);
	shader_set_int(framebuffer_get_shader(rb), shader_get_uniform_index(
		framebuffer_get_shader(rb), "in_Use_Texture_Albedo"),
		!(m->data.texture_albedo == -1));
	shader_set_mat4(framebuffer_get_shader(rb), shader_get_uniform_index(
		framebuffer_get_shader(rb), "in_Transform"), *mvp);
	glBindVertexArray(vg->v_arrayid);
	glDrawArrays(GL_TRIANGLES, 0, vg->v.length);
	glBindVertexArray(0);
}

static void		render_shadow_mesh(int mesh_index, int rb, MAT4 *view_proj)
{
	t_mesh		*m;
	unsigned	i;
	MAT4		mvp;
	t_transform	*t;

	m = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!m)
		return ;
	i = 0;
	while (i < m->vgroups.length)
	{
		t = ezarray_get_index(engine_get()->transforms, m->transform_index);
		mvp = mat4_mult_mat4(*view_proj, t->transform);
		render_shadow_vgroup(ezarray_get_index(m->vgroups, i), rb, &mvp);
		i++;
	}
}

void			render_shadow()
{
	FRUSTUM		frustum;
	MAT4		projection;
	MAT4		view;
	t_light		*light;
	unsigned	mesh_index;

	mesh_index = 0;
	frustum = full_scene_frustum();
	projection = mat4_orthographic(frustum, frustum.x * 1.5f, frustum.y * 1.5f);
	view = mat4_lookat(new_vec3(-1, 1, 0), new_vec3(0, 0, 0), UP);
	light = ezarray_get_index(engine_get()->lights, 0);
	framebuffer_bind(light->render_buffer);
	glClearDepthf(1);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	projection = mat4_mult_mat4(projection, view);
	while (mesh_index < engine_get()->meshes.length)
	{
		render_shadow_mesh(mesh_index, light->render_buffer, &projection);
		mesh_index++;
	}
}