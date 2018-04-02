/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader_set1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/02 16:36:43 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/02 16:37:04 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

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
