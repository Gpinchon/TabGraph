/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GLSL.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/21 16:37:40 by gpinchon          #+#    #+#             */
/*   Updated: 2019/01/05 08:58:34 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "parser/GLSL.hpp"
#include "parser/InternalTools.hpp"
#include <unistd.h>
#include <iostream>

static auto	deferredVertCode =
	#include "deferred.vert"
;
static auto	deferredFragCode = 
	#include "deferred.frag"
;
static auto	forwardVertCode = 
	#include "forward.vert"
;
static auto	forwardFragCode = 
	#include "forward.frag"
;

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

std::shared_ptr<Shader>	GLSL::compile(const std::string &name,
	const std::string &vertex_code, const std::string &fragment_code)
{
	auto	shader = std::static_pointer_cast<GLSL>(Shader::create(name));
	GLuint	vertexid = 0;
	GLuint	fragmentid = 0;

	try {
		vertexid = compile_shader_code(vertex_code, GL_VERTEX_SHADER);
		fragmentid = compile_shader_code(fragment_code, GL_FRAGMENT_SHADER);
		shader->link(vertexid, fragmentid);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error compiling ") + name + " :\n" + e.what());
	}
	shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
	shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	return (shader);
}

std::shared_ptr<Shader>	GLSL::parse(const std::string &name,
	const std::string &vertex_file_path,
	const std::string &fragment_file_path)
{
	return (GLSL::compile(name,
			file_to_str(vertex_file_path),
			file_to_str(fragment_file_path)));
}

std::shared_ptr<Shader>	GLSL::compile(const std::string &name,
	const std::string &vertex_code, const std::string &fragment_code, ShaderType type, const std::string &defines)
{
	auto	shader = std::static_pointer_cast<GLSL>(Shader::create(name));
	GLuint	vertexid = 0;
	GLuint	fragmentid = 0;
	try {
		if (ForwardShader == type)
		{
			vertexid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardVertCode + vertex_code, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardFragCode + fragment_code, GL_FRAGMENT_SHADER);
		}
		else if (LightingShader == type)
		{
			vertexid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredVertCode + vertex_code, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredFragCode + fragment_code, GL_FRAGMENT_SHADER);
		}
		else if (PostShader == type)
		{
			vertexid = compile_shader_code("#define POSTSHADER\n" + defines + deferredVertCode + vertex_code, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define POSTSHADER\n" + defines + deferredFragCode + fragment_code, GL_FRAGMENT_SHADER);
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
	const std::string &vertex_file_path,
	const std::string &fragment_file_path,
	ShaderType type,
	const std::string &defines)
{
	auto	vertex_code = file_to_str(vertex_file_path);
	auto	fragment_code = file_to_str(fragment_file_path);
	return (GLSL::compile(name, vertex_code, fragment_code, type, defines));
}

std::shared_ptr<Shader>	GLSL::compile(const std::string &name,
	const std::string &shader_code, ShaderType type, const std::string &defines)
{
	auto	shader = std::static_pointer_cast<GLSL>(Shader::create(name));
	static auto emptyShaderCode = 
		#include "empty.glsl"
	;

	GLuint	vertexid = 0;
	GLuint	fragmentid = 0;
	GLuint	computeid = 0;
	try {
		if (ForwardShader == type)
		{
			vertexid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardFragCode + shader_code, GL_FRAGMENT_SHADER);
			shader->link(vertexid, fragmentid);
		}
		else if (LightingShader == type)
		{
			vertexid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredFragCode + shader_code, GL_FRAGMENT_SHADER);
			shader->link(vertexid, fragmentid);
		}
		else if (PostShader == type)
		{
			vertexid = compile_shader_code("#define POSTSHADER\n" + defines + deferredVertCode + emptyShaderCode, GL_VERTEX_SHADER);
			fragmentid = compile_shader_code("#define POSTSHADER\n" + defines + deferredFragCode + shader_code, GL_FRAGMENT_SHADER);
			shader->link(vertexid, fragmentid);
		}
		else if (ComputeShader == type)
		{
			computeid = compile_shader_code(defines + shader_code, GL_COMPUTE_SHADER);
			shader->link(computeid);
		}
		
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error parsing ") + name + " :\n" + e.what());
	}
	shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
	shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
	glDeleteShader(vertexid);
	glDeleteShader(fragmentid);
	glDeleteShader(computeid);
	return (shader);
}

std::shared_ptr<Shader>	GLSL::parse(const std::string &name,
	const std::string &shader_file_path, ShaderType type, const std::string &defines)
{
	auto	shader_code = file_to_str(shader_file_path);
	return (GLSL::compile(name, shader_code, type, defines));
}
