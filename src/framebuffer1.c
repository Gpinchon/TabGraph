/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   framebuffer1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/17 16:14:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/09 19:58:06 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

VEC2			framebuffer_get_size(int framebuffer)
{
	t_framebuffer	*buffer;

	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer)
		return (window_get_size());
	return (buffer->size);
}

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

void			framebuffer_resize(int framebuffer, VEC2 new_size)
{
	t_framebuffer	*buffer;
	unsigned		i;

	VEC2	size = framebuffer_get_size(framebuffer);
	if (size.x == new_size.x && size.y == size.y)
		return ;
	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer)
		return ;
	framebuffer_bind(framebuffer);
	buffer->size = new_size;
	i = 0;
	while (i < buffer->color_attachements.length)
	{
		int	ti = framebuffer_get_attachement(framebuffer, i);
		t_texture *t = texture_get(ti);
		glDeleteTextures(1, &t->glid);
		t->size = new_size;
		glGenTextures(1, &t->glid);
		glBindTexture(t->target, t->glid);
		glTexImage2D(t->target, 0, t->internal_format, new_size.x, new_size.y, 0, t->format, GL_FLOAT, NULL);
		glBindTexture(t->target, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D, t->glid, 0);
		texture_set_parameters(ti, 5,
			(GLenum[5]){GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T,
			GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER,
			GL_TEXTURE_MAX_ANISOTROPY_EXT},
			(GLenum[5]){GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, ANISOTROPY});
		i++;
	}
	int ti = framebuffer_get_depth(framebuffer);
	if (ti != -1)
	{
		t_texture	*t = texture_get(ti);
		glDeleteTextures(1, &t->glid);
		glGenTextures(1, &t->glid);
		glBindTexture(t->target, t->glid);
		glTexImage2D(t->target, 0, t->internal_format, new_size.x, new_size.y, 0,
			t->format, GL_FLOAT, NULL);
		glBindTexture(t->target, 0);
		texture_set_parameters(ti, 5,
				(GLenum[5]){GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T,
				GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER,
				GL_TEXTURE_MAX_ANISOTROPY_EXT},
				(GLenum[5]){GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, ANISOTROPY});
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D, t->glid, 0);
	}
	framebuffer_bind(-1);
}