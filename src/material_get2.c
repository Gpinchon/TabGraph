/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_get2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/17 16:36:16 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:36:01 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

inline int	material_get_texture_albedo(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_albedo);
}

inline int	material_get_texture_specular(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_specular);
}

inline int	material_get_texture_roughness(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_roughness);
}

inline int	material_get_texture_metallic(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (-1);
	return (mtl->data.texture_metallic);
}
