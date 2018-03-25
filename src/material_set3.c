/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_set3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/25 20:00:57 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/25 20:01:09 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void		material_set_texture_height(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_height = value;
}

void		material_set_texture_ao(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_ao = value;
}

void		material_set_texture_stupid(int material_index, int value)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return ;
	mtl->data.texture_stupid = value;
}