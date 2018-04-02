/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_set1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/25 19:59:03 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/02 16:20:51 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void	material_set_metallic(int material_index, float value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.metallic = value;
}

void	material_set_alpha(int material_index, float value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.alpha = value;
}

void	material_set_parallax(int material_index, float value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.parallax = value;
}

void	material_set_stupidity(int material_index, float value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.stupidity = value;
}

void	material_set_texture_albedo(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_albedo = value;
}
