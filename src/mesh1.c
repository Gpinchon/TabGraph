/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/02 16:58:38 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/08 02:34:07 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void	mesh_rotate(int mesh_index, VEC3 rotation)
{
	t_mesh		*mesh;
	t_transform	*t;

	if (!(mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
		return ;
	t = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
	t->rotation = rotation;
}

void	mesh_scale(int mesh_index, VEC3 scale)
{
	t_mesh		*mesh;
	t_transform	*t;

	if (!(mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
		return ;
	t = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
	t->scaling = scale;
}

void	mesh_translate(int mesh_index, VEC3 position)
{
	t_mesh		*mesh;
	t_transform	*t;

	if (!(mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
		return ;
	t = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
	t->position = position;
}

t_mesh	*mesh_get(int mesh_index)
{
	return (ezarray_get_index(engine_get()->meshes, mesh_index));
}