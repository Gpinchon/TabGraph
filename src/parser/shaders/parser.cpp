/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/21 16:37:40 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/08 18:47:09 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "parser/GLSL.hpp"
#include "parser/InternalTools.hpp"
#include <unistd.h>
#include <iostream>

static std::string glslVersion = std::string("#version 450\n");
static auto	deferredVertCode = file_to_str(Engine::program_path() + "./res/shaders/deferred.vert");
static auto	deferredFragCode = file_to_str(Engine::program_path() + "./res/shaders/deferred.frag");
static auto	forwardVertCode = file_to_str(Engine::program_path() + "./res/shaders/forward.vert");
static auto	forwardFragCode = file_to_str(Engine::program_path() + "./res/shaders/forward.frag");
static auto emptyShaderCode = file_to_str(Engine::program_path() + "./res/shaders/empty.glsl");

GLuint	compile_shader_code(const std::string &code, GLenum type)
{
	GLuint	shaderid;

	auto	fullCode = glslVersion + code;
	auto	buf = fullCode.c_str();
	shaderid = glCreateShader(type);
	glShaderSource(shaderid, 1, &buf, nullptr);
	glCompileShader(shaderid);
	Shader::check_shader(shaderid);
	return (shaderid);
}

GLuint	compile_shader(const std::string &path, GLenum type)
{
	GLuint	shaderid;

	if (access(path.c_str(), R_OK) != 0) {
		throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
	}
	shaderid = compile_shader_code(file_to_str(path), type);
	Shader::check_shader(shaderid);
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
		throw std::runtime_error(std::string("Linking Error :\n") + e.what());
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
	const std::string &fragment_file_path,
	const std::string &vertex_file_path, ShaderType type, const std::string &defines)
{
	auto	shader = static_cast<GLSL*>(Shader::create(name));
	GLuint	vertexid = 0;
	GLuint	fragmentid = 0;
	try {
		if (ForwardShader == type)
		{
			vertexid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardVertCode + file_to_str(vertex_file_path), GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
		}
		else if (LightingShader == type)
		{
			vertexid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredVertCode + file_to_str(vertex_file_path), GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
		}
		else if (PostShader == type)
		{
			vertexid = compile_shader_code("#define POSTSHADER\n" + defines + deferredVertCode + file_to_str(vertex_file_path), GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define POSTSHADER\n" + defines + deferredFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
		}
		shader->link(vertexid, fragmentid);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error parsing ") + name + " :\n" + e.what());
	}
	shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
	shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
	glDetachShader(shader->_program, vertexid);
	glDetachShader(shader->_program, fragmentid);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}

Shader	*GLSL::parse(const std::string &name,
	const std::string &fragment_file_path, ShaderType type, const std::string &defines)
{
	auto	shader = static_cast<GLSL*>(Shader::create(name));
	GLuint	vertexid = 0;
	GLuint	fragmentid = 0;
	try {
		if (ForwardShader == type)
		{
			vertexid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
		}
		else if (LightingShader == type)
		{
			vertexid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
		}
		else if (PostShader == type)
		{
			vertexid = compile_shader_code("#define POSTSHADER\n" + defines + deferredVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define POSTSHADER\n" + defines + deferredFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
		}
		shader->link(vertexid, fragmentid);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error parsing ") + name + " :\n" + e.what());
	}
	shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
	shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
	glDetachShader(shader->_program, vertexid);
	glDetachShader(shader->_program, fragmentid);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}

Shader	*GLSL::parse(const std::string &name,
	const std::string &vertex_file_path,
	const std::string &fragment_file_path)
{
	auto	shader = static_cast<GLSL*>(Shader::create(name));
	GLuint	vertexid = 0;
	GLuint	fragmentid = 0;
	try {
		vertexid = compile_shader(vertex_file_path, GL_VERTEX_SHADER);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error compiling ") + vertex_file_path + " :\n" + e.what());
	}
	try {
		fragmentid = compile_shader(fragment_file_path, GL_FRAGMENT_SHADER);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error compiling ") + fragment_file_path + " :\n" + e.what());
	}
	try {
		shader->link(vertexid, fragmentid);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error linking ") + name + " :\n" + e.what());
	}
	shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
	shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
	glDetachShader(shader->_program, vertexid);
	glDetachShader(shader->_program, fragmentid);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}
