/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/05 14:13:13 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"

Texture::Texture(const std::string &name) : _glid(0), _data(nullptr), _loaded(false)
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
	if (s.x > 0 && s.y > 0)
		glTexImage2D(t->_target, 0, fi, s.x, s.y, 0, f, GL_FLOAT, NULL);
	if (f == GL_RGB)
		glTexParameteri(t->_target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexParameteri(t->_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(t->_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(t->_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glBindTexture(t->_target, 0);
	glObjectLabel(GL_TEXTURE, t->_glid, -1, name.c_str());
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

UCHAR	*Texture::data() const
{
	return (_data);
}

void	Texture::assign(Texture &dest_texture, GLenum target)
{
	glBindTexture(dest_texture._target, dest_texture._glid);
	glBindTexture(target, dest_texture._glid);
	glTexImage2D(target, 0, _internal_format, _size.x, _size.y, 0,
		_format, GL_UNSIGNED_BYTE, _data);
	glBindTexture(target, 0);
	glBindTexture(dest_texture._target, 0);
}

void	Texture::load()
{
	if (_loaded)
		return ;
	std::cout << "Texture::load() " << _name << " _internal_format " << _internal_format << "_format " << _format << std::endl;
	std::cout << "_target " << _target << std::endl;
	std::cout << "_size " << _size.x << " " << _size.y << std::endl;
	std::cout << "data " << &_data << std::endl;
	std::cout << "_glid " << _glid << std::endl;
	if (_size.x > MAXTEXRES || _size.y > MAXTEXRES)
		resize(new_vec2(MIN(_size.x, MAXTEXRES),
			MIN(_size.y, MAXTEXRES)));
	if (!_glid)
		glGenTextures(1, &_glid);
	glBindTexture(_target, _glid);
	if (_bpp < 32)
		glTexParameteri(_target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0,
		_format, GL_UNSIGNED_BYTE, _data);
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

UCHAR		Texture::bpp() const
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
	ULL			h;
	Texture	*t;

	i = 0;
	std::hash<std::string>	hash_fn;
	h = hash_fn(name);
	while ((t = Engine::texture(i)))
	{
		if (h == t->_id)
			return (t);
		i++;
	}
	return (nullptr);
}

VEC4	Texture::texelfetch(const VEC2 &uv)
{
	VEC4			value;
	char			opp;
	unsigned char	*p;
	int				i;

	value = new_vec4(0, 0, 0, 0);
	if (!_data)
		return (value);
	i = 0;
	auto nuv = new_vec2(
		CLAMP(round(_size.x * uv.x), 0, _size.x - 1),
		CLAMP(round(_size.y * uv.y), 0, _size.y - 1));
	opp = _bpp / 8;
	p = &_data[(int)(nuv.y * _size.x + nuv.x) * opp];
	while (i < opp)
	{
		((float*)&value)[i] = p[i];
		i++;
	}
	return (value);
}

void	Texture::set_pixel(const VEC2 &uv, const VEC4 &value)
{
	char			opp;
	unsigned char	*p;
	int				i;

	if (!_data)
		return ;
	i = 0;
	auto nuv = new_vec2(
		CLAMP(round(_size.x * uv.x), 0, _size.x - 1),
		CLAMP(round(_size.y * uv.y), 0, _size.y - 1));
	opp = _bpp / 8;
	p = &_data[(int)(nuv.y * _size.x + nuv.x) * opp];
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
	VEC4		value;

	value = new_vec4(0, 0, 0, 0);
	if (!_data)
		return (value);
	vt[0] = new_vec3(CLAMP(_size.x * uv.x, 0, _size.x - 1),
		CLAMP(_size.y * uv.y, 0, _size.y - 1), 0);
	auto nuv = new_vec2(fract(vt[0].x), fract(vt[0].y));
	vt[0].z = ((1 - nuv.x) * (1 - nuv.y));
	vt[1] = new_vec3(MIN(_size.x - 1, vt[0].x + 1),
		MIN(_size.y - 1, vt[0].y + 1), (nuv.x * (1 - nuv.y)));
	vt[2] = new_vec3(vt[0].x, vt[1].y, ((1 - nuv.x) * nuv.y));
	vt[3] = new_vec3(vt[1].x, vt[0].y, (nuv.x * nuv.y));
	s[0] = -1;
	while (++s[0] < (_bpp / 8))
	{
		s[1] = -1;
		while (++s[1] < 4)
			((float*)&value)[s[0]] += (&_data[(int)(round(vt[s[1]].y) *
			_size.x + round(vt[s[1]].x)) * (_bpp / 8)])[s[0]] * vt[s[1]].z;
	}
	return (value);
}

void	Texture::resize(const VEC2 &ns)
{
	unsigned char	*d;
	int				i[3];
	VEC4			v;
	VEC2			uv;

	if (_data)
	{
		d = new unsigned char[unsigned(ns.x * ns.y * (_bpp / 8) * sizeof(UCHAR))];
		i[0] = -1;
		while (++i[0] < ns.x)
		{
			i[1] = -1;
			while (++i[1] < ns.y)
			{
				uv = new_vec2(i[0] / (float)ns.x, i[1] / (float)ns.y);
				v = sample(uv);
				i[2] = -1;
				while (++i[2] < (_bpp / 8))
					(&d[(int)(i[1] * ns.x + i[0]) * (_bpp / 8)])[i[2]] =
					((float*)&v)[i[2]];
			}
		}
		delete _data;
		_data = d;
	}
	_size = ns;
	if (_glid)
	{
		glDeleteTextures(1, &_glid);
		glGenTextures(1, &_glid);
		glBindTexture(_target, _glid);
		glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0,
			_format, GL_FLOAT, _data);
		glBindTexture(_target, 0);
	}
}
