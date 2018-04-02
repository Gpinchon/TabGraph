/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader_set.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/02 16:35:57 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/02 16:37:12 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

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
