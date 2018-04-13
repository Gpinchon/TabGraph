/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_get1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/17 16:35:11 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:36:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

inline int	material_get_texture_emitting(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_emitting);
}

inline int	material_get_texture_normal(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_normal);
}

inline int	material_get_texture_height(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_height);
}

inline int	material_get_texture_ao(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_ao);
}

inline int	material_get_texture_stupid(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_stupid);
}
