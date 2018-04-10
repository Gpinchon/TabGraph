/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   framebuffer2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 16:30:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/10 16:50:44 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

static void	framebuffer_resize_attachement(int framebuffer, int attachement,
	VEC2 ns)
{
	int			ti;
	t_texture	*t;
	GLenum		g;

	(void)g;
	ti = framebuffer_get_attachement(framebuffer, attachement);
	t = texture_get(ti);
	glDeleteTextures(1, &t->glid);
	t->size = ns;
	glGenTextures(1, &t->glid);
	glBindTexture(t->target, t->glid);
	glTexImage2D(t->target, 0, t->internal_format, ns.x, ns.y, 0,
		t->format, GL_FLOAT, NULL);
	glBindTexture(t->target, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachement,
		GL_TEXTURE_2D, t->glid, 0);
	texture_set_parameters(ti, 5,
		(GLenum[5]){GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T,
		GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER,
		GL_TEXTURE_MAX_ANISOTROPY_EXT},
		(GLenum[5]){GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR,
			GL_LINEAR, ANISOTROPY});
}

static void	framebuffer_resize_depth(int framebuffer, VEC2 ns)
{
	int			ti;
	t_texture	*t;
	GLenum		g;

	(void)g;
	ti = framebuffer_get_depth(framebuffer);
	if (ti == -1)
		return ;
	t = texture_get(ti);
	glDeleteTextures(1, &t->glid);
	glGenTextures(1, &t->glid);
	glBindTexture(t->target, t->glid);
	glTexImage2D(t->target, 0, t->internal_format, ns.x, ns.y, 0,
		t->format, GL_FLOAT, NULL);
	glBindTexture(t->target, 0);
	texture_set_parameters(ti, 5,
			(GLenum[5]){GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T,
			GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER,
			GL_TEXTURE_MAX_ANISOTROPY_EXT},
			(GLenum[5]){GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR,
				GL_LINEAR, ANISOTROPY});
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, t->glid, 0);
}

void		framebuffer_resize(int framebuffer, VEC2 new_size)
{
	t_framebuffer	*buffer;
	unsigned		i;
	VEC2			size;

	size = framebuffer_get_size(framebuffer);
	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer || (size.x == new_size.x && size.y == size.y))
		return ;
	framebuffer_bind(framebuffer);
	buffer->size = new_size;
	i = 0;
	while (i < buffer->color_attachements.length)
	{
		framebuffer_resize_attachement(framebuffer, i, new_size);
		i++;
	}
	framebuffer_resize_depth(framebuffer, new_size);
	framebuffer_bind(-1);
}
