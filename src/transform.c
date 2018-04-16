/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transform.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:10:01 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 18:32:52 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

int		transform_create(VEC3 position, VEC3 rotation, VEC3 scale)
{
	t_transform	t;

	t = new_transform(position, rotation, scale, UP);
	transform_update(&t);
	ezarray_push(&engine_get()->transforms, &t);
	return (engine_get()->transforms.length - 1);
}

VEC3	transform_set_position(int transform_index, VEC3 position)
{
	t_transform	*t;

	t = ezarray_get_index(engine_get()->transforms, transform_index);
	if (!t)
		return (new_vec3(0, 0, 0));
	return (t->position = position);
}

VEC3	transform_get_position(int transform_index)
{
	t_transform	*t;

	t = ezarray_get_index(engine_get()->transforms, transform_index);
	if (!t)
		return (new_vec3(0, 0, 0));
	return (t->position);
}
