/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/17 16:29:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/17 16:29:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void	camera_set_target(int camera_index, int transform_index)
{
	t_camera	*camera;

	camera = ezarray_get_index(engine_get()->cameras, camera_index);
	if (!camera)
		return ;
	camera->target_index = transform_index;
}

void	camera_set_position(int camera_index, VEC3 position)
{
	t_camera	*camera;

	camera = ezarray_get_index(engine_get()->cameras, camera_index);
	if (!camera)
		return ;
	camera->position = position;
}
