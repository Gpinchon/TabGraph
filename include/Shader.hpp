/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/19 14:29:34 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include "Object.hpp"
#include <string>
#include <unordered_map>

class Texture;

struct	ShaderVariable
{
	size_t		id;
	std::string	name;
	GLint		size;
	GLenum		type;
	GLuint		loc;
};

class Shader : public Object
{
public:
	static Shader	*get_by_name(const std::string &);
    static Shader   *create(const std::string &);
	static bool		check_shader(const GLuint &id);
	static bool		check_program(const GLuint &id);
	GLuint			link(const GLuint &vertexid, const GLuint &fragmentid);
	GLuint			link(const GLuint &geometryid, const GLuint &vertexid, const GLuint &fragmentid);
	GLuint			link(const GLuint &shaderid);
	void			bind_texture(const std::string &,
					Texture *, const GLenum &texture_unit);
	void			unbind_texture(GLenum texture_unit);
	void            set_uniform(const std::string &uname, const bool &, unsigned nbr = 1);
    void			set_uniform(const std::string &uname, const int &, unsigned nbr = 1);
	void			set_uniform(const std::string &uname, const unsigned &, unsigned nbr = 1);
	void			set_uniform(const std::string &uname, const float &, unsigned nbr = 1);
	void			set_uniform(const std::string &uname, const VEC2 &, unsigned nbr = 1);
	void			set_uniform(const std::string &uname, const VEC3 &, unsigned nbr = 1);
	void			set_uniform(const std::string &uname, const MAT4 &, unsigned nbr = 1);
	void			use(const bool &use_program = true);
	ShaderVariable	*get_uniform(const std::string &name);
	bool			in_use();
protected:
	std::unordered_map<std::string, ShaderVariable>	_get_variables(GLenum type);
	GLuint		    _program;
	bool		    _in_use;
	std::unordered_map<std::string, ShaderVariable> _uniforms;
	std::unordered_map<std::string, ShaderVariable> _attributes;
private:
	Shader(const std::string &name);
};
