/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_get3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/17 16:37:05 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/17 16:38:29 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

inline float	material_get_roughness(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (0);
	return (mtl->data.roughness);
}

inline float	material_get_metallic(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (0);
	return (mtl->data.metallic);
}

inline float	material_get_alpha(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (0);
	return (mtl->data.alpha);
}

inline float	material_get_parallax(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (0);
	return (mtl->data.parallax);
}

inline float	material_get_stupidity(int material_index)
{
	t_material	*mtl;

	mtl = ezarray_get_index(engine_get()->materials, material_index);
	if (!mtl)
		return (0);
	return (mtl->data.stupidity);
}
