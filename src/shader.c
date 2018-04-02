/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:52:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/02 16:36:10 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void	shader_use(int shader_index)
{
	t_shader	*shader;

	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	if (!shader)
	{
		glUseProgram(0);
		return ;
	}
	glUseProgram(shader->program);
}

int		shader_get_by_name(char *name)
{
	int			i;
	ULL			h;
	t_shader	*s;

	i = 0;
	h = hash((unsigned char*)name);
	while ((s = ezarray_get_index(engine_get()->shaders, i)))
	{
		if (h == s->id)
			return (i);
		i++;
	}
	return (0);
}

int		shader_get_uniform_index(int shader_index, char *name)
{
	t_shader			*shader;
	t_shadervariable	*shader_v;
	unsigned			i;
	ULL					h;

	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	if (!shader)
		return (-1);
	i = 0;
	h = hash((unsigned char*)name);
	while (i < shader->uniforms.length)
	{
		shader_v = ezarray_get_index(shader->uniforms, i);
		if (shader_v && shader_v->id == h)
			return (i);
		i++;
	}
	return (-1);
}

void	shader_unbind_texture(int shader_index, GLenum texture_unit)
{
	shader_use(shader_index);
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP);
	shader_use(-1);
}

void	shader_bind_texture(int shader_index, int uniform_index,
			int texture_index, GLenum texture_unit)
{
	t_texture *texture;

	texture = texture_get(texture_index);
	if (!texture)
	{
		shader_unbind_texture(shader_index, texture_unit);
		shader_use(shader_index);
		shader_set_int(shader_index, uniform_index, texture_unit - GL_TEXTURE0);
		shader_use(-1);
		return ;
	}
	shader_use(shader_index);
	glActiveTexture(texture_unit);
	glBindTexture(texture->target, texture->glid);
	shader_set_int(shader_index, uniform_index, texture_unit - GL_TEXTURE0);
	shader_use(-1);
}
