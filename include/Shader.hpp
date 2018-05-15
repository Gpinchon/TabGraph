/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/15 21:07:32 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "vml.h"
#include "GLIncludes.hpp"
#include <string>
#include <map>

class Texture;

struct	ShaderVariable
{
	size_t		id;
	std::string	name;
	GLint		size;
	GLenum		type;
	GLuint		loc;
};

class Shader
{
public:
	static Shader	*get_by_name(const std::string &);
    static Shader   *create(const std::string &);
	static bool		check_shader(const GLuint &id);
	static bool		check_program(const GLuint &id);
	GLuint			link(const GLuint &vertexid, const GLuint &fragmentid);
	void			bind_texture(const std::string &,
					const Texture *, const GLenum &texture_unit);
	void			unbind_texture(GLenum texture_unit);
	void			set_name(const std::string &);
	void			set_uniform(const std::string &name, const Texture *, const int value);
	void            set_uniform(const std::string &, const bool &);
    void			set_uniform(const std::string &, const int &);
	void			set_uniform(const std::string &, const unsigned &);
	void			set_uniform(const std::string &, const float &);
	void			set_uniform(const std::string &, const VEC2 &);
	void			set_uniform(const std::string &, const VEC3 &);
	void			set_uniform(const std::string &, const MAT4 &);
	void			use(const bool &use_program = true);
	ShaderVariable	*get_uniform(const std::string &name);
	const std::string &name();
	bool			in_use();
protected:
	std::map<std::string, ShaderVariable>	_get_variables(GLenum type);
	size_t			_id;
	std::string	    _name;
	GLuint		    _program;
	bool		    _in_use;
	std::map<std::string, ShaderVariable> _uniforms;
	std::map<std::string, ShaderVariable> _attributes;
private:
	Shader(const std::string &name);
};
