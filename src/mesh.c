/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:35:57 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

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
		return ;
	while (i < mesh->vgroups.length)
	{
		vgroup_load(mesh_index, i);
		i++;
	}
}

void	mesh_update(int mesh_index)
{
	t_mesh		*mesh;

	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!mesh)
		return ;
	transform_update(ezarray_get_index(
		engine_get()->transforms, mesh->transform_index));
}

void	mesh_render(int mesh_index, int camera_index, RENDERTYPE type)
{
	t_mesh		*m;
	int			mtl;
	unsigned	vi;
	float		alpha;
	UCHAR		bpp;

	m = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!m)
		return ;
	vi = 0;
	while (vi < m->vgroups.length)
	{
		if (type)
		{
			mtl = ((t_vgroup*)ezarray_get_index(m->vgroups, vi))->mtl_index;
			alpha = material_get_alpha(mtl);
			bpp = texture_get_bpp(material_get_texture_albedo(mtl));
			if ((type == render_opaque && alpha == 1 && bpp < 32)
			|| (type == render_transparent && (alpha < 1 || bpp == 32)))
				vgroup_render(camera_index, mesh_index, vi);
		}
		else
			vgroup_render(camera_index, mesh_index, vi);
		vi++;
	}
}

void	mesh_center(int mesh_index)
{
	t_mesh		*m;
	unsigned	vi;

	m = ezarray_get_index(engine_get()->meshes, mesh_index);
	if (!m)
		return ;
	vi = 0;
	while (vi < m->vgroups.length)
	{
		vgroup_center(mesh_index, vi);
		vi++;
	}
	m->bounding_box.center = new_vec3(0, 0, 0);
	m->bounding_box.min =
		vec3_sub(m->bounding_box.min, m->bounding_box.center);
	m->bounding_box.max =
		vec3_sub(m->bounding_box.max, m->bounding_box.center);
}
