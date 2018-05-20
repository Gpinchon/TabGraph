/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 23:02:24 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include "vml.h"
#include <string>

class	Texture
{
public:
	static Texture	*create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi);
	static Texture	*get_by_name(const std::string &);
	void			resize(const VEC2 &ns);
	void			set_name(const std::string &);
	void			set_parameter(GLenum p, GLenum v);
	void			set_parameters(int p_nbr,
						GLenum *p, GLenum *v);
	void			assign(Texture &dest_texture,
						GLenum target);
	void			load();
	void			generate_mipmap();
	void			set_pixel(const VEC2 &uv, const VEC4 &value);
	void			blur(const int &pass, const float &radius);
	GLenum			target() const;
	void			format(GLenum *format,
						GLenum *internal_format);
	GLuint			glid() const;
	GLubyte	*data() const;
	GLubyte	bpp() const;
	VEC2			size() const;
	VEC4			texelfetch(const VEC2 &uv);
	VEC4			sample(const VEC2 &uv);
	const std::string	&name();
	const std::string	&name() const;
protected:
	size_t		_id{};
	GLuint		_glid;
	std::string	_name;
	VEC2		_size;
	char		_bpp;
	GLenum		_target;
	GLenum		_format;
	GLenum		_internal_format;
	GLubyte		*_data{nullptr};
	bool		_loaded;
	Texture(const std::string &name);
};
