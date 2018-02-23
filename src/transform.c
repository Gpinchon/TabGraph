/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transform.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:10:01 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/22 22:08:17 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int	transform_create(VEC3 position, VEC3 rotation, VEC3 scale)
{
	t_transform	t;

	t = new_transform(position, rotation, scale, UP);
	transform_update(&t);
	ezarray_push(&engine_get()->transforms, &t);
	return (engine_get()->transforms.length - 1);
}
