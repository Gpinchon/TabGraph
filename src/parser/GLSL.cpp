/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GLSL.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/21 16:37:40 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 17:23:28 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "parser/GLSL.hpp"
#include "parser/InternalTools.hpp"
#include <unistd.h>
#include <iostream>

//static std::string glslVersion = std::string("#version 440\n");
static auto	deferredVertCode = file_to_str(Engine::program_path() + "./res/shaders/deferred.vert");
static auto	deferredFragCode = file_to_str(Engine::program_path() + "./res/shaders/deferred.frag");
static auto	forwardVertCode = file_to_str(Engine::program_path() + "./res/shaders/forward.vert");
static auto	forwardFragCode = file_to_str(Engine::program_path() + "./res/shaders/forward.frag");
static auto emptyShaderCode = file_to_str(Engine::program_path() + "./res/shaders/empty.glsl");

GLuint	compile_shader_code(const std::string &code, GLenum type)
{
	GLuint	shaderid;
	auto	glslVersionString = std::string((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
	auto	glslVersionNbr = int(std::stof(glslVersionString) * 100);

	auto	fullCode = std::string("#version ") + std::to_string(glslVersionNbr) + "\n" + code;
	auto	codeBuff = fullCode.c_str();
	shaderid = glCreateShader(type);
	glShaderSource(shaderid, 1, &codeBuff, nullptr);
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

std::shared_ptr<Shader>	GLSL::parse(const std::string &name,
	const std::string &fragment_file_path,
	const std::string &vertex_file_path, ShaderType type, const std::string &defines)
{
	auto	shader = std::static_pointer_cast<GLSL>(Shader::create(name));
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
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}

std::shared_ptr<Shader>	GLSL::parse(const std::string &name,
	const std::string &fragment_file_path, ShaderType type, const std::string &defines)
{
	auto	shader = std::static_pointer_cast<GLSL>(Shader::create(name));
	GLuint	vertexid = 0;
	GLuint	fragmentid = 0;
	GLuint	computeid = 0;
	try {
		if (ForwardShader == type)
		{
			vertexid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
			shader->link(vertexid, fragmentid);
		}
		else if (LightingShader == type)
		{
			vertexid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
			shader->link(vertexid, fragmentid);
		}
		else if (PostShader == type)
		{
			vertexid = compile_shader_code("#define POSTSHADER\n" + defines + deferredVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define POSTSHADER\n" + defines + deferredFragCode + file_to_str(fragment_file_path), GL_FRAGMENT_SHADER);
			shader->link(vertexid, fragmentid);
		}
		else if (ComputeShader == type)
		{
			computeid = compile_shader_code(defines + file_to_str(fragment_file_path), GL_COMPUTE_SHADER);
			shader->link(computeid);
		}
		
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error parsing ") + name + " :\n" + e.what());
	}
	shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
	shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
	if (vertexid > 0) {
		glDeleteShader(vertexid);
	}
	if (fragmentid > 0) {
		glDeleteShader(fragmentid);
	}
	if (computeid > 0) {
		glDeleteShader(computeid);
	}
	return (shader);
}

std::shared_ptr<Shader>	GLSL::parse(const std::string &name,
	const std::string &vertex_file_path,
	const std::string &fragment_file_path)
{
	auto	shader = std::static_pointer_cast<GLSL>(Shader::create(name));
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
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}
