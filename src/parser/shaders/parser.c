/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/21 16:37:40 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/12 21:38:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <scope.h>

GLuint	compile_shader(const char *path, GLenum type)
{
	GLuint	shaderid;
	FILE	*stream;
	char	*buf;

	buf = ft_strjoin(engine_get()->program_path, path);
	if (access(buf, F_OK | W_OK)
	|| !(stream = fopen(buf, "rb")))
	{
		free(buf);
		return (0);
	}
	free(buf);
	buf = stream_to_str(stream);
	shaderid = glCreateShader(type);
	glShaderSource(shaderid, 1, (const char **)(&buf) , NULL);
	glCompileShader(shaderid);
	free(buf);
	fclose(stream);
	return (shaderid);
}

int		check_shader(GLuint id)
{
	char	*log;
	GLint	result;
	GLint	loglength;

	result = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &loglength);
	if (loglength > 1)
	{
		log = calloc(loglength, sizeof(char));
		glGetShaderInfoLog(id, loglength, NULL, &log[0]);
		puts("Error compiling shader : ");
		puts(log);
		free(log);
		return (-1);
	}
	return (0);
}

int		check_program(GLuint id)
{
	char	*log;
	GLint	result;
	GLint	loglength;

	result = GL_FALSE;
	glGetProgramiv(id, GL_LINK_STATUS, &result);
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglength);
	if (loglength > 0)
	{
		log = calloc(loglength, sizeof(char));
		glGetProgramInfoLog(id, loglength, NULL, &log[0]);
		puts(log);
		free(log);
		return (-1);
	}
	return (0);
}

GLuint		link_shaders(GLuint vertexid, GLuint fragmentid)
{
	GLuint progid = glCreateProgram();

	glAttachShader(progid, vertexid);
	glAttachShader(progid, fragmentid);
	glLinkProgram(progid);
	return(progid);
}

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

void		get_shader_loc(t_shader *shader)
{
	shader->uniforms = get_shader_uniforms(shader);
	shader->attributes = get_shader_attributes(shader);
}

int	load_shaders(const char *name, const char *vertex_file_path,
	const char *fragment_file_path)
{	
	GLuint		vertexid;
	GLuint		fragmentid;
	t_shader	shader;

	vertexid = compile_shader(vertex_file_path, GL_VERTEX_SHADER);
	fragmentid = compile_shader(fragment_file_path, GL_FRAGMENT_SHADER);
	if(!vertexid || !fragmentid)
	{
		puts("Impossible to open file !");
		return (-1);
	}
	if (check_shader(vertexid) || check_shader(fragmentid))
		return (-1);
	memset(&shader, 0, sizeof(t_shader));
	shader.program = link_shaders(vertexid, fragmentid);
	shader.name = new_ezstring(name);
	shader.id = hash((unsigned char *)name);
	get_shader_loc(&shader);
	glDetachShader(shader.program, vertexid);
	glDetachShader(shader.program, fragmentid);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	ezarray_push(&engine_get()->shaders, &shader);
	return (engine_get()->shaders.length - 1);
}