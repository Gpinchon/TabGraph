/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:52:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/10 01:03:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

Shader::Shader(const std::string &name) : _program(0), _in_use(false)
{
	set_name(name);
}

void	Shader::set_name(const std::string &name)
{
	std::hash<std::string>	hash_fn;
	_name = name;
	_id = hash_fn(name);
}

Shader	*Shader::create(const std::string &name)
{
	auto	shader = new Shader(name);
	Engine::add(*shader);
	return (shader);
}

const std::string	&Shader::name(void)
{
	return (_name);
}

ShaderVariable	*Shader::get_uniform(const std::string &name)
{
	auto	it = _uniforms.find(name);
	if (it != _uniforms.end())
		return (&it->second);
	return (nullptr);
}

void			Shader::set_uniform(const std::string &name, const Texture *, const int value)
{
	auto	v = get_uniform(name);
	if (!v)
		return ;
	bool	bound = in_use();
	if (!bound)
		use();
	glUniform1i(v->loc, value);
	if (!bound)
		use(false);
}

void			Shader::set_uniform(const std::string &name, const int &value)
{
	auto	v = get_uniform(name);
	if (!v)
		return ;
	bool	bound = in_use();
	if (!bound)
		use();
	glUniform1i(v->loc, value);
	if (!bound)
		use(false);
}

void			Shader::set_uniform(const std::string &name, const bool &value)
{
	auto	v = get_uniform(name);
	if (!v)
		return ;
	bool	bound = in_use();
	if (!bound)
		use();
	glUniform1i(v->loc, value);
	if (!bound)
		use(false);
}

void			Shader::set_uniform(const std::string &name, const unsigned &value)
{
	auto	v = get_uniform(name);
	if (!v)
		return ;
	bool	bound = in_use();
	if (!bound)
		use();
	glUniform1ui(v->loc, value);
	if (!bound)
		use(false);
}

void			Shader::set_uniform(const std::string &name, const float &value)
{
	auto	v = get_uniform(name);
	if (!v)
		return ;
	bool	bound = in_use();
	if (!bound)
		use();
	glUniform1f(v->loc, value);
	if (!bound)
		use(false);
}

void			Shader::set_uniform(const std::string &name, const VEC2 &value)
{
	auto	v = get_uniform(name);
	if (!v)
		return ;
	bool	bound = in_use();
	if (!bound)
		use();
	glUniform2f(v->loc, value.x, value.y);
	if (!bound)
		use(false);
}

void			Shader::set_uniform(const std::string &name, const VEC3 &value)
{
	auto	v = get_uniform(name);
	if (!v)
		return ;
	bool	bound = in_use();
	if (!bound)
		use();
	glUniform3f(v->loc, value.x, value.y, value.z);
	if (!bound)
		use(false);
}

void			Shader::set_uniform(const std::string &name, const MAT4 &value)
{
	auto	v = get_uniform(name);
	if (!v)
		return ;
	bool	bound = in_use();
	if (!bound)
		use();
	glUniformMatrix4fv(v->loc, 1, GL_FALSE, (float*)&value);
	if (!bound)
		use(false);
}

bool			Shader::in_use()
{
	return (_in_use);
}

void			Shader::use(const bool &use_program)
{
	if (!use_program)
	{
		glUseProgram(0);
		return ;
	}
	glUseProgram(_program);
	_in_use = true;
}

Shader		*Shader::get_by_name(const std::string &name)
{
	int			i;
	size_t			h;
	Shader	*s;

	i = 0;
	std::hash<std::string>	hash_fn;
	h = hash_fn(name);
	while ((s = Engine::shader(i)))
	{
		if (h == s->_id)
			return (s);
		i++;
	}
	return (nullptr);
}

void	Shader::unbind_texture(GLenum texture_unit)
{
	bool	bound = in_use();
	if (!bound)
		use();
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP);
	if (!bound)
		use(false);
}

void	Shader::bind_texture(const std::string &name,
			const Texture *texture, const GLenum &texture_unit)
{
	bool	bound = in_use();
	if (!bound)
		use();
	if (!texture)
		unbind_texture(texture_unit);
	else
	{
		glActiveTexture(texture_unit);
		glBindTexture(texture->target(), texture->glid());
	}
	set_uniform(name, texture, texture_unit - GL_TEXTURE0);
	if (!bound)
		use(false);
}
