/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_set2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/25 19:59:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/02 16:21:09 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void		material_set_texture_specular(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_specular = value;
}

void		material_set_texture_roughness(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_roughness = value;
}

void		material_set_texture_metallic(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_metallic = value;
}

void		material_set_texture_emitting(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_emitting = value;
}

void		material_set_texture_normal(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_normal = value;
}
