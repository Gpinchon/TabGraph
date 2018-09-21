/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:52:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 18:01:44 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

std::vector<std::shared_ptr<Shader>>	Shader::_shaders;

Shader::Shader(const std::string &name) : _program(0), _in_use(false)
{
	set_name(name);
}

std::shared_ptr<Shader>	Shader::create(const std::string &name)
{
	auto	shader = std::shared_ptr<Shader>(new Shader(name));
	_shaders.push_back(shader);
	return (shader);
}

std::shared_ptr<Shader>	Shader::get(unsigned index)
{
	if (index >= _shaders.size())
		return (nullptr);
	return (_shaders.at(index));
}

std::shared_ptr<Shader> Shader::get_by_name(const std::string &name)
{
	std::hash<std::string>	hash_fn;
	auto					h = hash_fn(name);
	for (auto s : _shaders) {
		if (h == s->id())
			return (s);
	}
	return (nullptr);
}

ShaderVariable	*Shader::get_uniform(const std::string &name)
{
	auto	it = _uniforms.find(name);
	if (it != _uniforms.end()) {
		return (&it->second);
	}
	return (nullptr);
}

void			Shader::set_uniform(const std::string &name, const int &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform1iv(v->loc, nbr, &value);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const bool &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	int	val = value;
	glUniform1iv(v->loc, nbr, &val);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const unsigned &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform1uiv(v->loc, nbr, &value);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const float &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform1fv(v->loc, nbr, &value);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const VEC2 &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform2fv(v->loc, nbr, &value.x);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const VEC3 &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform3fv(v->loc, nbr, &value.x);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const MAT4 &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniformMatrix4fv(v->loc, nbr, GL_FALSE, (float*)&value);
	if (!bound) {
		use(false);
	}
}

bool			Shader::in_use()
{
	return (_in_use);
}

void			Shader::use(const bool &use_program)
{
	if (!use_program) {
		glUseProgram(0);
		return ;
	}
	glUseProgram(_program);
	_in_use = true;
}

void	Shader::unbind_texture(GLenum texture_unit)
{
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP);
	if (!bound) {
		use(false);
	}
}

void	Shader::bind_texture(const std::string &name,
	std::shared_ptr<Texture> texture, const GLenum texture_unit)
{
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	if (texture == nullptr) {
		unbind_texture(texture_unit);
	}
	else {
		texture->load();
		glActiveTexture(texture_unit);
		glBindTexture(texture->target(), texture->glid());
	}
	set_uniform(name, int(texture_unit - GL_TEXTURE0));
	if (!bound) {
		use(false);
	}
}

GLuint	Shader::link(const GLuint shaderid)
{
	_program = glCreateProgram();
	glAttachShader(_program, shaderid);
	glLinkProgram(_program);
	try {
		check_program(_program);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Linking Error :\n") + e.what());
	}
	return (_program);
}

GLuint	Shader::link(const GLuint vertexid, const GLuint fragmentid)
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

GLuint	Shader::link(const GLuint geometryid, const GLuint vertexid, const GLuint fragmentid)
{
	_program = glCreateProgram();
	glAttachShader(_program, geometryid);
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

bool	Shader::check_shader(const GLuint id)
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

bool	Shader::check_program(const GLuint id)
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
