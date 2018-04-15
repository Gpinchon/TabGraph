/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 14:22:15 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 14:24:28 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parser.h>

ARRAY		get_shader_uniforms(t_shader *shader)
{
	t_shadervariable	v;
	ARRAY				uniforms;
	char				*name;
	GLint				ivcount;
	GLsizei				length;

	name = calloc(4096, sizeof(char));
	glGetProgramiv(shader->program, GL_ACTIVE_UNIFORMS, &ivcount);
	uniforms = new_ezarray(other, 0, sizeof(t_shadervariable));
	while (--ivcount >= 0)
	{
		name = memset(name, 0, sizeof(char) * 4096);
		glGetActiveUniform(shader->program, (GLuint)ivcount, 4096, &length,
			&v.size, &v.type, name);
		v.name = new_ezstring(name);
		v.id = hash((unsigned char *)name);
		v.loc = glGetUniformLocation(shader->program, name);
		ezarray_push(&uniforms, &v);
	}
	free(name);
	return (uniforms);
}

ARRAY		get_shader_attributes(t_shader *shader)
{
	t_shadervariable	v;
	ARRAY				attributes;
	char				*name;
	GLint				ivcount;
	GLsizei				length;

	name = calloc(4096, sizeof(char));
	glGetProgramiv(shader->program, GL_ACTIVE_ATTRIBUTES, &ivcount);
	attributes = new_ezarray(other, 0, sizeof(t_shadervariable));
	while (--ivcount >= 0)
	{
		name = memset(name, 0, sizeof(char) * 4096);
		glGetActiveAttrib(shader->program, (GLuint)ivcount, 4096, &length,
			&v.size, &v.type, name);
		v.name = new_ezstring(name);
		v.id = hash((unsigned char *)name);
		v.loc = glGetUniformLocation(shader->program, name);
		ezarray_push(&attributes, &v);
	}
	free(name);
	return (attributes);
}
