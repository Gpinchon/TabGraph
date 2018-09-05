/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/21 16:37:40 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/05 23:23:49 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "parser/GLSL.hpp"
#include "parser/InternalTools.hpp"
#include <unistd.h>
#include <iostream>

static std::string glslVersion = std::string("#version 450\n");

GLuint	compile_shader_code(const std::string &code, GLenum type)
{
	GLuint	shaderid;

	auto	fullCode = glslVersion + code;
	auto	buf = fullCode.c_str();
	shaderid = glCreateShader(type);
	glShaderSource(shaderid, 1, &buf, nullptr);
	glCompileShader(shaderid);
	try {
		Shader::check_shader(shaderid);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error compiling Shader : ") + " :\n" + e.what());
	}
	return (shaderid);
}

GLuint	compile_shader(const std::string &path, GLenum type)
{
	GLuint	shaderid;

	if (access(path.c_str(), R_OK) != 0) {
		throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
	}
	shaderid = compile_shader_code(file_to_str(path), type);
	try {
		Shader::check_shader(shaderid);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error compiling : ") + path + " :\n" + e.what());
	}
	return (shaderid);
}

std::unordered_map<std::string, ShaderVariable>		Shader::_get_variables(GLenum type)
{
	char				name[4096];
	GLint				ivcount;
	GLsizei				length;

	glGetProgramiv(_program, type, &ivcount);
	std::unordered_map<std::string, ShaderVariable> variables;
	while (--ivcount >= 0)
	{
		ShaderVariable	v;
		memset(name, 0, sizeof(char) * 4096);
		glGetActiveUniform(_program, static_cast<GLuint>(ivcount), 4096, &length,
			&v.size, &v.type, name);
		v.name = name;
		std::hash<std::string> hash_fn;
		v.id = hash_fn(name);
		v.loc = glGetUniformLocation(_program, name);
		variables.insert(std::make_pair(v.name, v));
	}
	return (variables);
}

GLuint	Shader::link(const GLuint &vertexid, const GLuint &fragmentid)
{
	_program = glCreateProgram();
	glAttachShader(_program, vertexid);
	glAttachShader(_program, fragmentid);
	glLinkProgram(_program);
	try {
		check_program(_program);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error linking  : ") + name() + " :\n" + e.what());
	}
	return (_program);
}

bool	Shader::check_shader(const GLuint &id)
{
	GLint	result;
	GLint	loglength;

	result = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &loglength);
	if (loglength > 1)
	{
		char	log[loglength];
		glGetShaderInfoLog(id, loglength, nullptr, &log[0]);
		throw std::runtime_error(log);
	}
	return (false);
}

bool	Shader::check_program(const GLuint &id)
{
	GLint	result;
	GLint	loglength;

	result = GL_FALSE;
	glGetProgramiv(id, GL_LINK_STATUS, &result);
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglength);
	if (loglength > 1)
	{
		char	log[loglength];
		glGetProgramInfoLog(id, loglength, nullptr, &log[0]);
		throw std::runtime_error(log);
	}
	return (false);
}

Shader	*GLSL::parse(const std::string &name,
	const std::string &fragment_file_path, ShaderType type)
{
	auto	shader = static_cast<GLSL*>(Shader::create(name));
	GLuint	vertexid = 0;
	GLuint	fragmentid = 0;
	static auto	passthroughVertex = compile_shader(Engine::program_path() + "./res/shaders/passthrough.vert", GL_VERTEX_SHADER);
	static auto	deferredFragCode = file_to_str(Engine::program_path() + "./res/shaders/deferred.frag");
	if (PostShader == type)
	{
		vertexid = passthroughVertex;
		fragmentid = compile_shader_code("#define POSTSHADER\n" + deferredFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
	}
	else if (LightingShader == type)
	{
		vertexid = passthroughVertex;
		fragmentid = compile_shader_code("#define LIGHTSHADER\n" + deferredFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
	}
	shader->link(vertexid, fragmentid);
	shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
	shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
	glDetachShader(shader->_program, vertexid);
	glDetachShader(shader->_program, fragmentid);
	//glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}

Shader	*GLSL::parse(const std::string &name,
	const std::string &vertex_file_path,
	const std::string &fragment_file_path)
{
	auto	shader = static_cast<GLSL*>(Shader::create(name));
	auto	vertexid = compile_shader(vertex_file_path, GL_VERTEX_SHADER);
	auto	fragmentid = compile_shader(fragment_file_path, GL_FRAGMENT_SHADER);
	shader->link(vertexid, fragmentid);
	shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
	shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
	glDetachShader(shader->_program, vertexid);
	glDetachShader(shader->_program, fragmentid);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}
