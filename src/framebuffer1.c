/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   framebuffer1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/17 16:14:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/17 16:15:25 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int				framebuffer_get_attachement(int framebuffer,
	int color_attachement)
{
	int				*attachement;
	t_framebuffer	*buffer;

	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer)
		return (-1);
	attachement = ezarray_get_index(buffer->color_attachements,
		color_attachement);
	if (!attachement)
		return (-1);
	return (*attachement);
}

int				framebuffer_get_depth(int framebuffer)
{
	t_framebuffer	*buffer;

	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer)
		return (-1);
	return (buffer->depth);
}

int				framebuffer_get_shader(int framebuffer)
{
	t_framebuffer	*buffer;

	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer)
		return (-1);
	return (buffer->shader);
}
