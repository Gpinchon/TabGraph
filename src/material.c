/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/01/19 14:15:46 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

t_material	new_material()
{
	t_material	mtl;

	ft_memset(&mtl, 0, sizeof(t_material));
	mtl.data.refraction = 1;
	mtl.data.metallic = 0;
	mtl.data.alpha = 1;
	mtl.data.texture_albedo = -1;
	mtl.data.texture_roughness = -1;
	mtl.data.texture_metallic = -1;
	mtl.data.texture_parallax = -1;
	mtl.data.texture_normal = -1;
	return (mtl);
}

int			get_material_index_by_name(ARRAY materials, char *name)
{
	int			i;
	ULL			h;
	t_material	*m;

	i = 0;
	h = hash((unsigned char*)name);
	while ((m = ezarray_get_index(materials, i)))
	{
		if (h == m->id)
			return (i);
		i++;
	}
	return (0);
}

int			get_material_index_by_id(ARRAY materials, ULL h)
{
	int			i;
	t_material	*m;

	i = 0;
	while ((m = ezarray_get_index(materials, i)))
	{
		if (h == m->id)
			return (i);
		i++;
	}
	return (0);
}