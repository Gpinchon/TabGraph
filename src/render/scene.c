/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 16:57:36 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 18:27:55 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

void	render_scene(int camera_index)
{
	unsigned	mesh_index;

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera_update(camera_index);
	mesh_index = 0;
	while (mesh_index < engine_get()->meshes.length)
	{
		mesh_update(mesh_index);
		mesh_render(mesh_index, camera_index, render_opaque);
		mesh_index++;
	}
	mesh_index = 0;
	while (mesh_index < engine_get()->meshes.length)
	{
		mesh_render(mesh_index, camera_index, render_transparent);
		mesh_index++;
	}
}
