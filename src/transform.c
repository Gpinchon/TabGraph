/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transform.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:10:01 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/23 11:45:13 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

int	transform_create(VEC3 position, VEC3 rotation, VEC3 scale)
{
	t_transform	*t;

	t = new t_transform;
	*t = new_transform(position, rotation, scale, UP);
	transform_update(t);
	engine_get()->transforms.push_back(t);
	return (engine_get()->transforms.size() - 1);
}

VEC3	transform_set_position(int transform_index, VEC3 position)
{
	t_transform	*t;

	t = engine_get()->transforms[transform_index];
	if (!t)
		return (new_vec3(0, 0, 0));
	return(t->position = position);
}

VEC3	transform_get_position(int transform_index)
{
	t_transform	*t;

	t = engine_get()->transforms[transform_index];
	if (!t)
		return (new_vec3(0, 0, 0));
	return(t->position);
}