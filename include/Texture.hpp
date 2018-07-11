/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/08 18:22:37 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include "vml.h"
#include <string>

class	Texture
{
public:
	static Texture	*create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi,  GLenum data_format = GL_UNSIGNED_BYTE);
	static Texture	*get_by_name(const std::string &);
	static size_t	get_data_size(GLenum data_type);
	static size_t	get_bpp(GLenum texture_format, GLenum data_type);
	virtual bool	is_loaded();
	virtual void	resize(const VEC2 &ns);
	virtual void	set_name(const std::string &);
	virtual void	set_parameter(GLenum p, GLenum v);
	virtual void	set_parameters(int p_nbr,
						GLenum *p, GLenum *v);
	virtual void	assign(Texture &dest_texture,
						GLenum target);
	virtual void	load();
	virtual void	generate_mipmap();
	virtual void	blur(const int &pass, const float &radius);
	virtual GLenum	target() const;
	virtual void	format(GLenum *format,
						GLenum *internal_format);
	virtual	GLenum	data_format();
	virtual size_t		data_size();
	virtual GLuint		glid() const;
	virtual void		*data() const;
	virtual GLubyte		bpp() const;
	virtual VEC2		size() const;
	virtual void	set_pixel(const VEC2 &uv, const GLubyte *value);
	virtual GLubyte		*texelfetch(const VEC2 &uv);
	virtual VEC4		sample(const VEC2 &uv);
	const std::string	&name();
	const std::string	&name() const;
	template <typename T>
	T					*at(float u, float v);
protected:
	size_t		_id{};
	GLuint		_glid;
	std::string	_name;
	VEC2		_size;
	char		_bpp;
	size_t		_data_size;
	GLenum		_data_format;
	GLenum		_target;
	GLenum		_format;
	GLenum		_internal_format;
	GLubyte		*_data{nullptr};
	bool		_loaded;
	Texture(const std::string &name);
};

template <typename T>
T	*Texture::at(float u, float v)
{
	auto	nuv = new_vec2(
		CLAMP(round(_size.x * u), 0, _size.x - 1),
		CLAMP(round(_size.y * v), 0, _size.y - 1));
	auto	opp = _bpp / 8;
	return (&_data[static_cast<int>(nuv.y * _size.x + nuv.x) * opp * sizeof(T)]);
}
