/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   framebuffer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/22 21:56:32 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/17 16:14:55 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void			framebuffer_bind(int framebuffer)
{
	t_framebuffer	*buffer;
	VEC2			size;

	if (framebuffer == -1)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		size = window_get_size();
		glViewport(0, 0, size.x, size.y);
		return ;
	}
	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer)
		return ;
	glBindFramebuffer(GL_FRAMEBUFFER, buffer->id);
	glViewport(0, 0, buffer->size.x, buffer->size.y);
}

int				framebuffer_create_attachement(int framebuffer,
	GLenum format, GLenum iformat)
{
	int				a;
	t_framebuffer	*b;

	b = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!b)
		return (-1);
	framebuffer_bind(framebuffer);
	a = texture_create(b->size, GL_TEXTURE_2D, format, iformat);
	texture_set_parameters(a, 2,
			(GLenum[2]){GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
			(GLenum[2]){GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE});
	if (format == GL_DEPTH_COMPONENT)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, texture_get_glid(a), 0);
		b->depth = a;
	}
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0
			+ b->color_attachements.length, GL_TEXTURE_2D,
			texture_get_glid(a), 0);
	ezarray_push(&b->color_attachements, &a);
	framebuffer_bind(-1);
	return (a);
}

void			framebuffer_setup_attachements(int fi)
{
	t_framebuffer	*b;
	unsigned		i;
	GLenum			a;
	GLenum			format[2];
	ARRAY			color_attachements;

	b = ezarray_get_index(engine_get()->framebuffers, fi);
	if (!b)
		return ;
	i = 0;
	color_attachements = new_ezarray(other, 0, sizeof(GLenum));
	while (i < b->color_attachements.length)
	{
		texture_get_format(framebuffer_get_attachement(fi, i), &format[0],
			&format[1]);
		a = GL_COLOR_ATTACHMENT0 + i;
		if (format[0] != GL_DEPTH_ATTACHMENT)
			ezarray_push(&color_attachements, &a);
		i++;
	}
	framebuffer_bind(fi);
	glDrawBuffers(color_attachements.length, color_attachements.data);
	framebuffer_bind(-1);
	destroy_ezarray(&color_attachements);
}

int				framebuffer_create(VEC2 size, int shader,
	int color_attachements, int depth)
{
	t_framebuffer	f;
	int				fi;
	int				i;
	GLenum			attachements[color_attachements];

	f.depth = -1;
	f.size = size;
	f.shader = shader;
	f.color_attachements = new_ezarray(INT, 0);
	glGenFramebuffers(1, &f.id);
	ezarray_push(&engine_get()->framebuffers, &f);
	fi = engine_get()->framebuffers.length - 1;
	i = -1;
	while (++i < color_attachements)
	{
		framebuffer_create_attachement(fi, GL_RGBA, GL_RGBA16F_ARB);
		attachements[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	if (depth)
		framebuffer_create_attachement(fi, GL_DEPTH_COMPONENT,
			GL_DEPTH_COMPONENT24);
	framebuffer_setup_attachements(fi);
	return (fi);
}
