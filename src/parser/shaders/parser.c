/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/21 16:37:40 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 14:32:38 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parser.h>

static GLuint	compile_shader(const char *path, GLenum type)
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

static GLuint	link_shaders(GLuint vertexid, GLuint fragmentid)
{
	GLuint progid = glCreateProgram();

	glAttachShader(progid, vertexid);
	glAttachShader(progid, fragmentid);
	glLinkProgram(progid);
	return(progid);
}

static int		check_shader(GLuint id)
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
		puts("Error compiling shader : \n");
		puts(log);
		free(log);
		return (-1);
	}
	return (0);
}

static int		check_program(GLuint id)
{
	char	*log;
	GLint	result;
	GLint	loglength;

	result = GL_FALSE;
	glGetProgramiv(id, GL_LINK_STATUS, &result);
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglength);
	if (loglength > 1)
	{
		log = calloc(loglength, sizeof(char));
		glGetProgramInfoLog(id, loglength, NULL, &log[0]);
		puts(log);
		free(log);
		return (-1);
	}
	return (0);
}

int				load_shaders(const char *name, const char *vertex_file_path,
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
	memset(&shader, 0, sizeof(t_shader));
	if (check_shader(vertexid) || check_shader(fragmentid) || 
		check_program(shader.program = link_shaders(vertexid, fragmentid)))
		return (-1);
	shader.name = new_ezstring(name);
	shader.id = hash((unsigned char *)name);
	shader.uniforms = get_shader_uniforms(&shader);
	shader.attributes = get_shader_attributes(&shader);
	glDetachShader(shader.program, vertexid);
	glDetachShader(shader.program, fragmentid);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	ezarray_push(&engine_get()->shaders, &shader);
	return (engine_get()->shaders.length - 1);
}
