/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:30:02 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/22 22:08:23 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int		camera_get_target_index(int camera_index)
{
	t_camera	*camera;

	camera = ezarray_get_index(engine_get()->cameras, camera_index);
	if (!camera)
		return (-1);
	return (camera->target_index);
}

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
		return;
	camera->position = position;
}

int		camera_create(float fov)
{
	t_camera	camera;

	ft_memset(&camera, 0, sizeof(t_camera));
	camera.fov = fov;
	camera.target_index = -1;
	ezarray_push(&engine_get()->cameras, &camera);
	return (engine_get()->cameras.length - 1);
}


void	camera_orbite(int camera_index, float phi, float theta, float radius)
{
	VEC3	target_position;
	VEC3	new_position;

	t_transform *target = ezarray_get_index(engine_get()->transforms, camera_get_target_index(camera_index));
	if (target)
		target_position = target->position;
	else
		target_position = new_vec3(0, 0, 0);
	new_position.x = target_position.x + radius * sin(phi) * cos(theta);
	new_position.z = target_position.z + radius * sin(phi) * sin(theta);
	new_position.y = target_position.y + radius * cos(phi);
	camera_set_position(camera_index, new_position);
}

void	camera_update(int camera_index)
{
	t_transform	*camera_target;
	VEC3		target;
	t_camera	*camera;
	
	camera = ezarray_get_index(engine_get()->cameras, camera_index);
	if (!camera)
		return ;
	camera_target = ezarray_get_index(engine_get()->transforms, camera->target_index);
	target = camera_target ? camera_target->position : new_vec3(0, 0, 0);
	camera->view = mat4_lookat(camera->position, target, UP);
	camera->projection = mat4_perspective(camera->fov, (float)WIDTH / (float)HEIGHT, 0.1, 1000);
	(void)camera_index;
}
