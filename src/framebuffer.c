/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   framebuffer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/22 21:56:32 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/08 16:49:10 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void			framebuffer_bind(int framebuffer)
{
	t_framebuffer	*buffer;

	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer)
		return ;
	glBindFramebuffer(GL_FRAMEBUFFER, buffer->id);
	glViewport(0, 0, buffer->size.x, buffer->size.y);
}

int				framebuffer_get_attachement(int framebuffer, int color_attachement)
{
	int				*attachement;
	t_framebuffer	*buffer;

	buffer = ezarray_get_index(engine_get()->framebuffers, framebuffer);
	if (!buffer)
		return (-1);
	attachement = ezarray_get_index(buffer->color_attachements, color_attachement);
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

int				framebuffer_create(VEC2 size, int shader, int color_attachements, int depth)
{
	t_framebuffer	f;
	int				i;
	int				*attachement;
	GLenum			attachements[color_attachements];

	f.size = size;
	f.shader = shader;
	f.color_attachements = new_ezarray(INT, color_attachements);
	glGenFramebuffers(1, &f.id);
	glBindFramebuffer(GL_FRAMEBUFFER, f.id);
	i = 0;
	while (i < color_attachements)
	{
		attachement = ezarray_get_index(f.color_attachements, i);
		*attachement = texture_create(f.size, GL_TEXTURE_2D, GL_RGBA16F_ARB, GL_RGBA);
		texture_set_parameters(*attachement, 4, 
			(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
			(GLenum[4]){GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE});
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture_get_ogl_id(*attachement), 0);
		attachements[i] = GL_COLOR_ATTACHMENT0 + i;
		i++;
	}
	if (depth)
	{
		f.depth = texture_create(f.size, GL_TEXTURE_2D, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT);
		texture_set_parameters(f.depth, 4,
			(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
			(GLenum[4]){GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE});
		/*texture_set_parameters(f.depth, 6,
			(GLenum[6]){GL_TEXTURE_COMPARE_FUNC, GL_TEXTURE_COMPARE_MODE, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
			(GLenum[6]){GL_LEQUAL, GL_COMPARE_REF_TO_TEXTURE, GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP});*/
		glDrawBuffers(color_attachements, attachements);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_get_ogl_id(f.depth), 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ezarray_push(&engine_get()->framebuffers, &f);
	return (engine_get()->framebuffers.length - 1);
}
