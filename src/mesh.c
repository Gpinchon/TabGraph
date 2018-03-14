/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/14 23:24:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int		mesh_get_transform_index(int mesh_index)
{
	t_mesh *mesh;

	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!mesh)
		return (-1);
	return (mesh->transform_index);
}

void	mesh_load(int mesh_index)
{
	GLuint	i;
	t_mesh	*mesh;

	i = 0;
	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!mesh)
		return;
	while (i < mesh->vgroups.length)
	{
		vgroup_load(mesh_index, i);
		i++;
	}
}

void	mesh_sort_draw(int mesh_index, int camera_index)
{
	MAT4		t;
	t_camera	*c;
	t_mesh		*m;
	t_vgroup	*v[2];
	unsigned	i;

	m = ezarray_get_index(engine_get()->meshes, mesh_index);
	c = ezarray_get_index(engine_get()->cameras, camera_index);
	if (!m || !c)
		return;
	t = ((t_transform*)ezarray_get_index(engine_get()->transforms, m->transform_index))->transform;
	i = 0;
	while (i < m->vgroups.length - 1)
	{
		v[0] = ezarray_get_index(m->vgroups, i + 0);
		v[1] = ezarray_get_index(m->vgroups, i + 1);
		if (vec3_distance(c->position, mat4_mult_vec3(t, v[0]->bounding_box.center)) <
		vec3_distance(c->position, mat4_mult_vec3(t, v[1]->bounding_box.center)))
		{
			t_vgroup temp;
			temp = *v[0];
			*v[0] = *v[1];
			*v[1] = temp;
			i = 0;
		}
		i++;
	}
}

void	mesh_update(int mesh_index)
{
	t_mesh		*mesh;

	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!mesh)
		return ;
	transform_update(ezarray_get_index(engine_get()->transforms, mesh->transform_index));
}

void	mesh_render(int mesh_index, int camera_index, RENDERTYPE type)
{
	t_mesh		*mesh;
	int			material;
	unsigned	vgroup_index;
	float		alpha;
	UCHAR		bpp;

	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!mesh)
		return ;
	vgroup_index = 0;
	while (vgroup_index < mesh->vgroups.length)
	{
		if (type)
		{
			material = ((t_vgroup*)ezarray_get_index(mesh->vgroups, vgroup_index))->mtl_index;
			alpha = material_get_alpha(material);
			bpp = texture_get_bpp(material_get_texture_albedo(material));
			if (type == render_opaque && alpha == 1 && bpp < 32)
				vgroup_render(camera_index, mesh_index, vgroup_index);
			else if (type == render_transparent && (alpha < 1 || bpp == 32))
				vgroup_render(camera_index, mesh_index, vgroup_index);
		}
		else
			vgroup_render(camera_index, mesh_index, vgroup_index);
		vgroup_index++;
	}
}

void	mesh_rotate(int mesh_index, VEC3 rotation)
{
	t_mesh		*mesh;
	t_transform	*transform;

	if (!(mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
		return ;
	transform = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
	transform->rotation = rotation;
}

void	mesh_scale(int mesh_index, VEC3 scale)
{
	t_mesh		*mesh;
	t_transform	*transform;

	if (!(mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
		return ;
	transform = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
	transform->scaling = scale;
}

void	mesh_translate(int mesh_index, VEC3 position)
{
	t_mesh		*mesh;
	t_transform	*transform;

	if (!(mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
		return ;
	transform = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
	transform->position = position;
}

void	mesh_center(int mesh_index)
{
	t_mesh		*mesh;
	unsigned	vgroup_index;

	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!mesh)
		return ;
	vgroup_index = 0;
	while (vgroup_index < mesh->vgroups.length)
	{
		vgroup_center(mesh_index, vgroup_index);
		vgroup_index++;
	}
	mesh->bounding_box.center = new_vec3(0, 0, 0);
	mesh->bounding_box.min = vec3_sub(mesh->bounding_box.min, mesh->bounding_box.center);
	mesh->bounding_box.max = vec3_sub(mesh->bounding_box.max, mesh->bounding_box.center);
}
