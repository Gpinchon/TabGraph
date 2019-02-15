/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 15:37:15 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include "Object.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class	Framebuffer;

class	Texture : public Object
{
public:
	static std::shared_ptr<Texture>	create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format = GL_UNSIGNED_BYTE, void *data = nullptr);
	static std::shared_ptr<Texture>	get_by_name(const std::string &);
	static std::shared_ptr<Texture>	get(unsigned index);
	static size_t					get_data_size(GLenum data_type);
	static size_t					get_bpp(GLenum texture_format, GLenum data_type);
	virtual std::shared_ptr<Texture>	shared_from_this();
	virtual bool	is_loaded();
	virtual void	resize(const VEC2 &ns);
	virtual void	set_parameteri(GLenum p, int v);
	virtual void	set_parameterf(GLenum p, float v);
	virtual void	restore_parameters();
	virtual void	assign(Texture &dest_texture, GLenum target);
	virtual void	load();
	virtual void	unload();
	virtual void	generate_mipmap();
	virtual void	blur(const int &pass, const float &radius);
	virtual GLenum	target() const;
	virtual void	format(GLenum *format, GLenum *internal_format);
	virtual GLenum	format();
	virtual GLenum	internal_format();
	virtual	GLenum	data_format();
	virtual size_t	data_size();
	virtual GLuint	glid() const;
	virtual void	*data() const;
	virtual GLubyte	bpp() const;
	virtual VEC2	size() const;
	virtual void	set_pixel(const VEC2 &uv, const VEC4 value);
	virtual void	set_pixel(const VEC2 &uv, const GLubyte *value);
	virtual GLubyte	*texelfetch(const VEC2 &uv);
	virtual VEC4	sample(const VEC2 &uv);
	virtual	size_t	values_per_pixel();
	template <typename T>
	T				*at(float u, float v);
protected:
	Texture(const std::string &name);
	Texture(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format = GL_UNSIGNED_BYTE, void *data = nullptr);
	static std::vector<std::shared_ptr<Texture>>	_textures;
	GLuint		_glid;
	VEC2		_size;
	char		_bpp;
	size_t		_data_size;
	GLenum		_data_format;
	GLenum		_target;
	GLenum		_format;
	GLenum		_internal_format;
	GLubyte		*_data{nullptr};
	bool		_loaded{false};
	std::shared_ptr<Framebuffer>	_blur_buffer0;
	std::shared_ptr<Framebuffer>	_blur_buffer1;
	std::shared_ptr<Framebuffer>	_generate_blur_buffer(const std::string &);
	std::unordered_map<GLenum, int>	_parametersi;
	std::unordered_map<GLenum, float>	_parametersf;
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
