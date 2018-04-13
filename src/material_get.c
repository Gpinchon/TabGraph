/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_get.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/17 16:32:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:36:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

inline VEC3	material_get_albedo(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (new_vec3(0, 0, 0));
	return (mtl->data.albedo);
}

inline VEC3	material_get_specular(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (new_vec3(0, 0, 0));
	return (mtl->data.specular);
}

inline VEC3	material_get_emitting(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (new_vec3(0, 0, 0));
	return (mtl->data.emitting);
}

inline VEC2	material_get_uv_scale(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (new_vec2(0, 0));
	return (mtl->data.uv_scale);
}
