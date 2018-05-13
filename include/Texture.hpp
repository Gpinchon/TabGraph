#pragma once

#include "GLIncludes.hpp"
#include "vml.h"
#include <string>

class	Texture
{
public:
	static Texture	*create(const std::string &name, VEC2 size, GLenum target, GLenum format, GLenum internal_format);
	static Texture	*get_by_name(const std::string &);
	void			resize(const VEC2 &new_size);
	void			set_name(const std::string &);
	void			set_parameter(GLenum p, GLenum v);
	void			set_parameters(int parameter_nbr,
						GLenum *parameters, GLenum *values);
	void			assign(Texture &texture,
						GLenum target);
	void			load();
	void			generate_mipmap();
	void			set_pixel(const VEC2 &uv, const VEC4 &value);
	void			blur(const int &pass, const float &radius);
	GLenum			target() const;
	void			format(GLenum *format,
						GLenum *internal_format);
	GLuint			glid() const;
	unsigned char	*data() const;
	unsigned char	bpp() const;
	VEC2			size() const;
	VEC4			texelfetch(const VEC2 &uv);
	VEC4			sample(const VEC2 &uv);
	const std::string	&name();
	const std::string	&name() const;
protected:
	size_t		_id;
	GLuint		_glid;
	std::string	_name;
	VEC2		_size;
	char		_bpp;
	GLenum		_target;
	GLenum		_format;
	GLenum		_internal_format;
	unsigned char   *_data;
	bool		_loaded;
	Texture(const std::string &name);
};
