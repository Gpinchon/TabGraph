/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/21 16:37:40 by gpinchon          #+#    #+#             */
/*   Updated: 2017/03/26 23:31:40 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <scope.h>

GLuint	compile_shader(const char *path, GLenum type)
{
	GLuint	shaderid;
	int		fd;
	char	*buf;

	buf = ft_strjoin(g_program_path, path);
	if (access(buf, F_OK | W_OK)
	|| (fd = open(buf, O_RDONLY)) <= 0)
	{
		free(buf);
		return (0);
	}
	free(buf);
	buf = file_to_str(fd);
	shaderid = glCreateShader(type);
	glShaderSource(shaderid, 1, (const char **)(&buf) , NULL);
	glCompileShader(shaderid);
	free(buf);
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
	if (loglength > 0)
	{
		log = ft_memalloc(sizeof(char) * loglength);
		glGetShaderInfoLog(id, loglength, NULL, &log[0]);
		ft_putendl(log);
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
		log = ft_memalloc(sizeof(char) * loglength);
		glGetProgramInfoLog(id, loglength, NULL, &log[0]);
		ft_putendl(log);
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

void		get_shader_loc(t_shader *shader)
{
	t_shadervariable	v;
	ARRAY	uniforms;
	ARRAY	attributes;
	GLint	ivcount;
	GLsizei	length;
	char	*name;

	name = malloc(sizeof(char) * 4096);
	glGetProgramiv(shader->program, GL_ACTIVE_ATTRIBUTES, &ivcount);
	attributes = new_ezarray(other, 0, sizeof(t_shadervariable));
	while (--ivcount >= 0)
	{
		name = ft_memset(name, sizeof(char) * 4096, 0);
		glGetActiveAttrib(shader->program, (GLuint)ivcount, 4096, &length, &v.size, &v.type, name);
		v.name = new_ezstring(name);
		v.id = hash((unsigned char *)name);
		v.loc = glGetUniformLocation(shader->program, name);
		ezarray_push(&attributes, &v);
	}
	glGetProgramiv(shader->program, GL_ACTIVE_UNIFORMS, &ivcount);
	uniforms = new_ezarray(other, 0, sizeof(t_shadervariable));
	while (--ivcount >= 0)
	{
		name = ft_memset(name, sizeof(char) * 4096, 0);
		glGetActiveUniform(shader->program, (GLuint)ivcount, 4096, &length, &v.size, &v.type, name);
		v.name = new_ezstring(name);
		v.id = hash((unsigned char *)name);
		v.loc = glGetUniformLocation(shader->program, name);
		ezarray_push(&uniforms, &v);
	}
	shader->uniforms = uniforms;
	shader->attributes = attributes;
}

t_shader	load_shaders(const char *vertex_file_path,const char *fragment_file_path)
{	
	GLuint		vertexid = compile_shader(vertex_file_path, GL_VERTEX_SHADER);
	GLuint		fragmentid = compile_shader(fragment_file_path, GL_FRAGMENT_SHADER);
	t_shader	shader;

	ft_memset(&shader, sizeof(t_shader), 0);
	if(!vertexid || !fragmentid)
	{
		ft_putendl("Impossible to open file !");
		return (shader);
	}
	if (check_shader(vertexid) || check_shader(fragmentid))
		return (shader);
	shader.program = link_shaders(vertexid, fragmentid);
	get_shader_loc(&shader);
	glDetachShader(shader.program, vertexid);
	glDetachShader(shader.program, fragmentid);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}