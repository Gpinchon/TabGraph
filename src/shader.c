/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:52:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/08 00:30:42 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void	shader_use(t_engine *engine, int shader_index)
{
	t_shader	*shader;

	shader = ezarray_get_index(engine->shaders, shader_index);
	if (!shader)
		return;
	glUseProgram(shader->program);
}

int		shader_get_by_name(t_engine *engine, char *name)
{
	int			i;
	ULL			h;
	t_shader	*s;

	i = 0;
	h = hash((unsigned char*)name);
	while ((s = ezarray_get_index(engine->shaders, i)))
	{
		if (h == s->id)
			return (i);
		i++;
	}
	return (0);
}

void	shader_set_uniform(t_engine *engine, int shader_index, int uniform_index, void *value)
{
	t_shadervariable	*variable;
	t_shader			*shader;

	shader = ezarray_get_index(engine->shaders, shader_index);
	if (!shader)
		return ;
	variable = ezarray_get_index(shader->uniforms, uniform_index);
	if (!variable)
		return ;
	if (variable->type == GL_FLOAT_VEC3)
		glUniform3fv(variable->loc, 1, ((float*)value));
	else if (variable->type == GL_FLOAT_VEC2)
		glUniform2fv(variable->loc, 1, ((float*)value));
	else if (variable->type == GL_FLOAT_MAT4)
		glUniformMatrix4fv(variable->loc, 1, GL_FALSE, ((float*)value));
	else if ((variable->type == GL_INT || variable->type == GL_BOOL || variable->type == GL_SAMPLER_2D || variable->type == GL_SAMPLER_CUBE))
		glUniform1i(variable->loc, *((int*)value));
	else if (variable->type == GL_UNSIGNED_INT)
		glUniform1ui(variable->loc, *((int*)value));
	else if (variable->type == GL_FLOAT)
		glUniform1f(variable->loc, *((float*)value));
}

int		shader_get_uniform_index(t_engine *engine, int shader_index, char *name)
{
	t_shader			*shader;
	unsigned			i;
	ULL					h;

	shader = ezarray_get_index(engine->shaders, shader_index);
	if (!shader)
		return (-1);
	i = 0;
	h = hash((unsigned char*)name);
	while (i < shader->uniforms.length)
	{
		if (((t_shadervariable*)ezarray_get_index(shader->uniforms, i))->id == h)
			return (i);
		i++;
	}
	return (-1);
}

void	shader_set_texture(t_engine *engine, int shader_index, int uniform_index, int texture_index, GLenum texture_unit)
{
	t_texture *texture;

	texture = ezarray_get_index(engine->textures, texture_index);
	if (!texture)
		return;
	glActiveTexture(texture_unit);
	glBindTexture(texture->target, texture->id_ogl);
	texture_unit -= GL_TEXTURE0;
	shader_set_uniform(engine, shader_index, uniform_index, &texture_unit);
}