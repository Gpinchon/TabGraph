/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:52:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/17 17:15:12 by gpinchon         ###   ########.fr       */
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
		return;
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

inline void	shader_set_int(int shader_index, int uniform_index, int value)
{
	t_shadervariable	*v;
	t_shader			*shader;

	shader_use(shader_index);
	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	if (!shader)
		return ;
	v = ezarray_get_index(shader->uniforms, uniform_index);
	if (!v)
		return ;
	glUniform1i(v->loc, value);
}

inline void	shader_set_uint(int shader_index, int uniform_index, unsigned value)
{
	t_shadervariable	*variable;
	t_shader			*shader;

	shader_use(shader_index);
	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	if (!shader)
		return ;
	variable = ezarray_get_index(shader->uniforms, uniform_index);
	if (!variable)
		return ;
	glUniform1ui(variable->loc, value);
}

inline void	shader_set_float(int shader_index, int uniform_index, float value)
{
	t_shadervariable	*variable;
	t_shader			*shader;

	shader_use(shader_index);
	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	if (!shader)
		return ;
	variable = ezarray_get_index(shader->uniforms, uniform_index);
	if (!variable)
		return ;
	glUniform1f(variable->loc, value);
}

inline void	shader_set_vec2(int shader_index, int uniform_index, VEC2 value)
{
	t_shadervariable	*variable;
	t_shader			*shader;

	shader_use(shader_index);
	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	if (!shader)
		return ;
	variable = ezarray_get_index(shader->uniforms, uniform_index);
	if (!variable)
		return ;
	glUniform2f(variable->loc, value.x, value.y);
}

inline void	shader_set_vec3(int shader_index, int uniform_index, VEC3 value)
{
	t_shadervariable	*variable;
	t_shader			*shader;

	shader_use(shader_index);
	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	if (!shader)
		return ;
	variable = ezarray_get_index(shader->uniforms, uniform_index);
	if (!variable)
		return ;
	glUniform3f(variable->loc, value.x, value.y, value.z);
}

inline void	shader_set_mat4(int shader_index, int uniform_index, MAT4 value)
{
	t_shadervariable	*variable;
	t_shader			*shader;

	shader_use(shader_index);
	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	if (!shader)
		return ;
	variable = ezarray_get_index(shader->uniforms, uniform_index);
	if (!variable)
		return ;
	glUniformMatrix4fv(variable->loc, 1, GL_FALSE, (float*)&value);
}

void	shader_set_uniform(int shader_index, int uniform_index, void *value)
{
	t_shadervariable	*variable;
	t_shader			*shader;

	shader_use(shader_index);
	shader = ezarray_get_index(engine_get()->shaders, shader_index);
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
	else if ((variable->type == GL_INT || variable->type == GL_BOOL || variable->type == GL_SAMPLER_2D || variable->type == GL_SAMPLER_CUBE || variable->type == GL_SAMPLER_2D_SHADOW))
		glUniform1i(variable->loc, *((int*)value));
	else if (variable->type == GL_UNSIGNED_INT)
		glUniform1ui(variable->loc, *((int*)value));
	else if (variable->type == GL_FLOAT)
		glUniform1f(variable->loc, *((float*)value));
}

int		shader_get_uniform_index(int shader_index, char *name)
{
	t_shader			*shader;
	unsigned			i;
	ULL					h;

	shader = ezarray_get_index(engine_get()->shaders, shader_index);
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

void	shader_unbind_texture(int shader_index, GLenum texture_unit)
{
	shader_use(shader_index);
	glActiveTexture(texture_unit);
	glDisable (GL_TEXTURE_2D);
	glDisable (GL_TEXTURE_CUBE_MAP);
}

void	shader_bind_texture(int shader_index, int uniform_index, int texture_index, GLenum texture_unit)
{
	t_texture *texture;

	texture = texture_get(texture_index);
	if (!texture)
	{
		shader_unbind_texture(shader_index, texture_unit);
		return;
	}
	shader_use(shader_index);
	glActiveTexture(texture_unit);
	glBindTexture(texture->target, texture->glid);
	shader_set_int(shader_index, uniform_index, texture_unit - GL_TEXTURE0);
}
