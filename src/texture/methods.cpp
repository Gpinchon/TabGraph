/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/06/09 12:53:01 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Texture.hpp"

Texture::Texture(const std::string &name) :
	_glid(0),
	_size(new_vec2(0, 0)),
	_bpp(0),
	_target(0),
	_format(0),
	_internal_format(0),
	_data(nullptr),
	_loaded(false)
{
	set_name(name);
}

void	Texture::set_name(const std::string &name)
{
	_name = name;
	std::hash<std::string> hash_fn;
	_id = hash_fn(name);
}

Texture		*Texture::create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi)
{
	Texture	*t;

	t = new Texture(name);
	t->_target = target;
	t->_format = f;
	t->_internal_format = fi;
	t->_size = s;
	glGenTextures(1, &t->_glid);
	glBindTexture(t->_target, t->_glid);
	if (s.x > 0 && s.y > 0) {
		glTexImage2D(t->_target, 0, fi, s.x, s.y, 0, f, GL_FLOAT, nullptr);
}
	if (f == GL_RGB) {
		glTexParameteri(t->_target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
}
	glTexParameteri(t->_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(t->_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(t->_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glBindTexture(t->_target, 0);
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, t->_glid, -1, name.c_str());
#endif //GL_DEBUG
	Engine::add(*t);
	return (t);
}

GLenum	Texture::target() const
{
	return (_target);
}

VEC2	Texture::size() const
{
	return (_size);
}

GLubyte	*Texture::data() const
{
	return (_data);
}

void	Texture::assign(Texture &dest_texture, GLenum target)
{
	glBindTexture(_target, _glid);
	glBindTexture(dest_texture._target, dest_texture._glid);
	glTexImage2D(target, 0, dest_texture._internal_format, dest_texture._size.x, dest_texture._size.y, 0,
		dest_texture._format, GL_UNSIGNED_BYTE, dest_texture._data);
	glBindTexture(_target, 0);
	glBindTexture(dest_texture._target, 0);
}

void	Texture::load()
{
	if (_loaded) {
		return ;
}
	if (_size.x > MAXTEXRES || _size.y > MAXTEXRES) {
		resize(new_vec2(std::min(int(_size.x), MAXTEXRES),
			std::min(int(_size.y), MAXTEXRES)));
}
	if (_glid == 0u) {
		glGenTextures(1, &_glid);
}
	glBindTexture(_target, _glid);
	if (_bpp < 32) {
		glTexParameteri(_target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
}
	if (_size.x > 0 && _size.y > 0) {
		glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0,
			_format, GL_UNSIGNED_BYTE, _data);
}
	glGenerateMipmap(_target);
	glBindTexture(_target, 0);
	_loaded = true;
}

void	Texture::generate_mipmap()
{
	set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(_target, _glid);
	glGenerateMipmap(_target);
	glBindTexture(_target, 0);
}

void		Texture::format(GLenum *format, GLenum *internal_format)
{
	*format = _format;
	*internal_format = _internal_format;
}

const std::string	&Texture::name()
{
	return (_name);
}

const std::string	&Texture::name() const
{
	return (_name);
}

GLubyte		Texture::bpp() const
{
	return (_bpp);
}

GLuint		Texture::glid() const
{
	return (_glid);
}

Texture		*Texture::get_by_name(const std::string &name)
{
	int			i;
	size_t			h;
	Texture	*t;

	i = 0;
	std::hash<std::string>	hash_fn;
	h = hash_fn(name);
	while ((t = Engine::texture(i)) != nullptr)
	{
		if (h == t->_id) {
			return (t);
}
		i++;
	}
	return (nullptr);
}

VEC4	Texture::texelfetch(const VEC2 &uv)
{
	VEC4			value{};
	char			opp;
	GLubyte	*p;
	int				i;

	value = new_vec4(0, 0, 0, 0);
	if (_data == nullptr) {
		return (value);
}
	i = 0;
	auto nuv = new_vec2(
		CLAMP(round(_size.x * uv.x), 0, _size.x - 1),
		CLAMP(round(_size.y * uv.y), 0, _size.y - 1));
	opp = _bpp / 8;
	p = &_data[static_cast<int>(nuv.y * _size.x + nuv.x) * opp];
	while (i < opp)
	{
		(reinterpret_cast<float*>(&value))[i] = p[i];
		i++;
	}
	return (value);
}

void	Texture::set_pixel(const VEC2 &uv, const VEC4 &value)
{
	char			opp;
	GLubyte	*p;
	int				i;

	if (_data == nullptr) {
		return ;
}
	i = 0;
	auto nuv = new_vec2(
		CLAMP(round(_size.x * uv.x), 0, _size.x - 1),
		CLAMP(round(_size.y * uv.y), 0, _size.y - 1));
	opp = _bpp / 8;
	p = &_data[static_cast<int>(nuv.y * _size.x + nuv.x) * opp];
	while (i < opp)
	{
		p[i] = ((float*)&value)[i] * 255.f;
		i++;
	}
}

void	Texture::set_parameter(GLenum p, GLenum v)
{
	glBindTexture(_target, _glid);
	glTexParameteri(_target, p, v);
	glBindTexture(_target, 0);
}

void	Texture::set_parameters(int p_nbr, GLenum *p, GLenum *v)
{
	glBindTexture(_target, _glid);
	while (p_nbr > 0)
	{
		glTexParameteri(_target, p[p_nbr - 1], v[p_nbr - 1]);
		p_nbr--;
	}
	glBindTexture(_target, 0);
}

VEC4	Texture::sample(const VEC2 &uv)
{
	int			s[2];
	VEC3		vt[4];
	VEC4		value{};

	value = new_vec4(0, 0, 0, 0);
	if (_data == nullptr) {
		return (value);
	}
	vt[0] = new_vec3(CLAMP(_size.x * uv.x, 0, _size.x - 1),
		CLAMP(_size.y * uv.y, 0, _size.y - 1), 0);
	auto nuv = new_vec2(fract(vt[0].x), fract(vt[0].y));
	vt[0].z = ((1 - nuv.x) * (1 - nuv.y));
	vt[1] = new_vec3(std::min(_size.x - 1, vt[0].x + 1),
		std::min(_size.y - 1, vt[0].y + 1), (nuv.x * (1 - nuv.y)));
	vt[2] = new_vec3(vt[0].x, vt[1].y, ((1 - nuv.x) * nuv.y));
	vt[3] = new_vec3(vt[1].x, vt[0].y, (nuv.x * nuv.y));
	s[0] = -1;
	while (++s[0] < (_bpp / 8))
	{
		s[1] = -1;
		while (++s[1] < 4) {
			(reinterpret_cast<float*>(&value))[s[0]] += (&_data[static_cast<int>(round(vt[s[1]].y) *
			_size.x + round(vt[s[1]].x)) * (_bpp / 8)])[s[0]] * vt[s[1]].z;
		}
	}
	return (value);
}

void	Texture::resize(const VEC2 &ns)
{
	GLubyte	*d;
	int				x, y, z;
	VEC4			v{};
	VEC2			uv{};

	if (_data != nullptr)
	{
		d = new GLubyte[unsigned(ns.x * ns.y * (_bpp))];
		x = 0;
		while (x < ns.x)
		{
			y = 0;
			while (y < ns.y)
			{
				uv = new_vec2(x / ns.x, y / ns.y);
				v = sample(uv);
				z = 0;
				while (z < (_bpp / 8))
				{
					(&d[int(y * ns.x + x) * (_bpp / 8)])[z] =
					(reinterpret_cast<float*>(&v))[z];
					z++;
				}
				y++;
			}
			x++;
		}
		delete [] _data;
		_data = d;
	}
	_size = ns;
	if (_glid != 0u)
	{
		glDeleteTextures(1, &_glid);
		glGenTextures(1, &_glid);
		glBindTexture(_target, _glid);
		glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0,
			_format, GL_FLOAT, _data);
		glBindTexture(_target, 0);
	}
}
