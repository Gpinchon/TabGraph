/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/10 17:12:45 by gpinchon         ###   ########.fr       */
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

void	mesh_render(int camera_index, int mesh_index)
{
	t_mesh		*mesh;
	unsigned	vgroup_index;

	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!mesh)
		return ;
	transform_update(ezarray_get_index(engine_get()->transforms, mesh->transform_index));
	vgroup_index = 0;
	while (vgroup_index < mesh->vgroups.length)
	{
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
