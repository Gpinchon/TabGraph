/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 15:50:41 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 16:56:10 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"

/*static void		find_mesh_minmax(VEC3 *min, VEC3 *max)
{
	VEC3		curmin;
	VEC3		curmax;
	t_mesh		*mesh;
	int			mesh_index;
	t_transform	*t;

	mesh_index = 0;
	while ((mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
	{
		t = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
		curmin = mat4_mult_vec3(t->transform, mesh->bounding_box.min);
		curmax = mat4_mult_vec3(t->transform, mesh->bounding_box.max);
		min->x = curmin.x < min->x ? curmin.x : min->x;
		min->y = curmin.y < min->y ? curmin.y : min->y;
		min->z = curmin.z < min->z ? curmin.z : min->z;
		max->x = curmax.x > max->x ? curmax.x : max->x;
		max->y = curmax.y > max->y ? curmax.y : max->y;
		max->z = curmax.z > max->z ? curmax.z : max->z;
		mesh_index++;
	}
}

static FRUSTUM	full_scene_frustum(void)
{
	float		value;
	VEC3		v[2];

	v[0] = new_vec3(1000, 1000, 1000);
	v[1] = new_vec3(-1000, -1000, -1000);
	find_mesh_minmax(&v[0], &v[1]);
	value = std::max(1.5, vec3_distance(v[0], v[1]) / 2.f);
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

void			render_shadow(int light_index)
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
	light = ezarray_get_index(engine_get()->lights, light_index);
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
}*/
