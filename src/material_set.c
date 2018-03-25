/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_set.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/25 19:58:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/25 19:58:45 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void	material_set_albedo(int material_index, VEC3 value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.albedo = value;
}

void	material_set_specular(int material_index, VEC3 value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.specular = value;
}

void	material_set_emitting(int material_index, VEC3 value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.emitting = value;
}

void	material_set_uv_scale(int material_index, VEC2 value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.uv_scale = value;
}

void	material_set_roughness(int material_index, float value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.roughness = value;
}