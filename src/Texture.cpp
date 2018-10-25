/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/10/25 11:41:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Engine.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "Render.hpp"
#include "parser/GLSL.hpp"
#include "Errors.hpp"

std::vector<std::shared_ptr<Texture>>	Texture::_textures;

Texture::Texture(const std::string &name) : Object(name),
_glid(0),
_size(new_vec2(0, 0)),
_bpp(0),
_data_size(0),
_data_format(GL_UNSIGNED_BYTE),
_target(0),
_format(0),
_internal_format(0),
_data(nullptr),
_loaded(false)
{	
}

Texture::Texture(const std::string &iname, VEC2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format, void *data) : Texture(iname)
{
	_target = target;
	_format = f;
	_internal_format = fi;
	_data_format = data_format;
	_data_size = get_data_size(data_format);
	_data = static_cast<GLubyte*>(data);
	_bpp = get_bpp(f, data_format);
	_size = s;
}

std::shared_ptr<Texture>	Texture::create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format, void *data)
{
	auto	t = std::shared_ptr<Texture>(new Texture(name, s, target, f, fi, data_format, data));
	glGenTextures(1, &t->_glid);
	glBindTexture(target, t->_glid);
	glBindTexture(target, 0);
	t->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	t->set_parameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, Config::Anisotropy());
	if (t->values_per_pixel() < 4) {
		t->set_parameteri(GL_TEXTURE_SWIZZLE_A, GL_ONE);
	}
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, t->_glid, -1, t->name().c_str());
	glCheckError();
#endif //GL_DEBUG
	_textures.push_back(t);
	return (t);
}

std::shared_ptr<Texture>	Texture::get_by_name(const std::string &name)
{
	std::hash<std::string>	hash_fn;
	auto					h = hash_fn(name);
	for (auto t : _textures) {
		if (h == t->id())
			return (t);
	}
	return (nullptr);
}

std::shared_ptr<Texture>	Texture::shared_from_this()
{
	return (std::static_pointer_cast<Texture>(Object::shared_from_this()));
}

size_t	Texture::get_bpp(GLenum texture_format, GLenum data_format)
{
	auto	data_size = Texture::get_data_size(data_format);
	switch (texture_format)
	{
		case GL_RED :
		return (1 * 8 * data_size);
		case GL_RGB :
		case GL_BGR :
		return (3 * 8 * data_size);
		case GL_RGBA :
		case GL_BGRA :
		return (4 * 8 * data_size);
		default : return (0);
	}
}

size_t	Texture::get_data_size(GLenum data_format)
{
	switch (data_format)
	{
		case GL_FLOAT :
		case GL_FIXED :
		case GL_INT :
		case GL_UNSIGNED_INT :
		return (sizeof(GLfloat));
		case GL_BYTE :
		case GL_UNSIGNED_BYTE :
		return (sizeof(GLubyte));
		case GL_HALF_FLOAT :
		case GL_SHORT :
		case GL_UNSIGNED_SHORT :
		return (sizeof(GLushort));
		case GL_DOUBLE :
		return (sizeof(GLdouble));
		default : return (0);
	}
}

GLenum	Texture::target() const
{
	return (_target);
}

VEC2	Texture::size() const
{
	return (_size);
}

void	*Texture::data() const
{
	return (_data);
}

void	Texture::assign(Texture &dest_texture, GLenum target)
{
	if (!_loaded)
		load();
	glBindTexture(_target, _glid);
	glBindTexture(dest_texture._target, dest_texture._glid);
	glTexImage2D(target, 0, dest_texture._internal_format, dest_texture._size.x, dest_texture._size.y, 0,
		dest_texture._format, dest_texture._data_format, dest_texture._data);
	glBindTexture(_target, 0);
	glBindTexture(dest_texture._target, 0);
}

void	Texture::unload()
{
	if (!_loaded) {
		return ;
	}
	glDeleteTextures(1, &_glid);
	_loaded = false;
	_glid = 0u;
}

void	Texture::load()
{
	if (_loaded) {
		return ;
	}
	if (Config::MaxTexRes() > 0 && _data && (_size.x > Config::MaxTexRes() || _size.y > Config::MaxTexRes())) {
		resize(new_vec2(
			std::min(int16_t(_size.x), Config::MaxTexRes()),
			std::min(int16_t(_size.y), Config::MaxTexRes())));
	}
	if (_glid == 0u) {
		glGenTextures(1, &_glid);
	}
	glBindTexture(_target, _glid);
	if (_size.x > 0 && _size.y > 0) {
		glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0, _format, _data_format, _data);
	}
	glBindTexture(_target, 0);
	for (auto p : _parametersi) {
		glTextureParameteri(_glid, p.first, p.second);
	}
	for (auto p : _parametersf) {
		glTextureParameterf(_glid, p.first, p.second);
	}
	generate_mipmap();
	_loaded = true;
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, _glid, -1, name().c_str());
#endif //GL_DEBUG
}

void	Texture::generate_mipmap()
{
	glBindTexture(_target, _glid);
	glGenerateMipmap(_target);
	glBindTexture(_target, 0);
}

void		Texture::format(GLenum *format, GLenum *internal_format)
{
	*format = _format;
	*internal_format = _internal_format;
}

GLubyte		Texture::bpp() const
{
	return (_bpp);
}

GLuint		Texture::glid() const
{
	return (_glid);
}

GLenum	Texture::internal_format()
{
	return (_internal_format);
}

GLenum	Texture::format()
{
	return (_format);
}

GLenum	Texture::data_format()
{
	return (_data_format);
}

size_t	Texture::data_size()
{
	return (_data_size);
}

size_t	Texture::values_per_pixel()
{
	return (_data_size ? _bpp / _data_size / 8 : 0);
}

GLubyte	*Texture::texelfetch(const VEC2 &uv)
{
	int				opp;

	if (_data == nullptr) {
		return (nullptr);
	}
	auto	nuv = new_vec2(
		CLAMP(uv.x, 0, int(_size.x - 1)),
		CLAMP(uv.y, 0, int(_size.y - 1)));
	opp = _bpp / 8;
	return (&_data[int(_size.x * nuv.y  + nuv.x) * opp]);
}

void	Texture::set_pixel(const VEC2 &uv, const VEC4 value)
{
	int			opp;
	VEC4		val{0, 0, 0, 1};

	opp = _bpp / 8;
	val = value;
	if (_data == nullptr) {
		_data = new GLubyte[int(_size.x * _size.y) * opp];
	}
	GLubyte			*p;
	p = texelfetch(vec2_mult(uv, _size));
	auto	valuePtr = reinterpret_cast<float*>(&val);
	for (auto i = 0, j = 0; i < int(opp / _data_size) && j < 4; ++i, ++j)
	{
		if (_data_size == 1)
			p[i] = valuePtr[j] * 255.f;
		else if (_data_size == sizeof(GLfloat))
			static_cast<GLfloat*>((void*)p)[i] = valuePtr[j];
	}
}

void	Texture::set_pixel(const VEC2 &uv, const GLubyte *value)
{
	int			opp;

	opp = _bpp / 8;
	if (_data == nullptr) {
		_data = new GLubyte[int(_size.x * _size.y) * opp];
	}
	GLubyte			*p;
	p = texelfetch(vec2_mult(uv, _size));
	for (auto i = 0; i < opp; ++i) {
		p[i] = value[i];
	}
}

void	Texture::set_parameterf(GLenum p, float v)
{
	_parametersf[p] = v;
	if (_glid == 0u)
		return ;
	glTextureParameterf(_glid, p, v);
#ifdef GL_DEBUG
	glCheckError();
#endif //GL_DEBUG
}

void	Texture::set_parameteri(GLenum p, int v)
{
	_parametersi[p] = v;
	if (_glid == 0u)
		return ;
	glTextureParameteri(_glid, p, v);
#ifdef GL_DEBUG
	glCheckError();
#endif //GL_DEBUG
}

VEC4	Texture::sample(const VEC2 &uv)
{
	VEC3		vt[4];
	VEC4		value{0, 0, 0, 1};

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
	auto	opp = _bpp / 8;
	for (auto i = 0; i < 4; ++i)
	{
		auto	d = &_data[int(round(vt[i].y) * _size.x + round(vt[i].x)) * opp];
		for (auto j = 0; j < int(opp / _data_size); ++j)
		{
			if (_data_size == 1)
				reinterpret_cast<float*>(&value)[j] += (d[j] * vt[i].z) / 255.f;
			else if (_data_size == sizeof(GLfloat))
				reinterpret_cast<float*>(&value)[j] += static_cast<float*>((void*)d)[j] * vt[i].z;
		}
	}
	return (value);
}

bool	Texture::is_loaded()
{
	return (_loaded);
}

#include <iostream>

void	Texture::resize(const VEC2 &ns)
{
	GLubyte	*d;

	_loaded = false;
	if (_data != nullptr)
	{
		auto	opp = _bpp / 8;
		d = new GLubyte[unsigned(ns.x * ns.y * opp)];
		for (auto	y = 0; y < ns.y; ++y)
		{
			for (auto	x = 0; x < ns.x; ++x)
			{
				auto	uv = new_vec2(x / ns.x, y / ns.y);
				auto	value = sample(uv);
				auto	p = &d[int(ns.x * y + x) * opp];
				for (auto z = 0; z < int(opp / _data_size); ++z) {
					if (_data_size == 1)
						p[z] = reinterpret_cast<float*>(&value)[z] * 255.f;
					else if (_data_size == sizeof(GLfloat))
						reinterpret_cast<float*>(p)[z] = reinterpret_cast<float*>(&value)[z];
				}
			}
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
		glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0, _format, _data_format, _data);
		glBindTexture(_target, 0);
		for (auto p : _parametersi) {
			glTextureParameteri(_glid, p.first, p.second);
		}
		for (auto p : _parametersf) {
			glTextureParameterf(_glid, p.first, p.second);
		}
	}
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, _glid, -1, name().c_str());
	glCheckError();
#endif //GL_DEBUG
}


std::shared_ptr<Framebuffer>	Texture::_generate_blur_buffer(const std::string &bname)
{
	auto	buffer = Framebuffer::create(bname, size(), 0, 0);
	buffer->create_attachement(_format, _internal_format);
	buffer->setup_attachements();
	return (buffer);
}

void	Texture::blur(const int &pass, const float &radius)
{
	if (pass == 0)
		return ;
	if (_blur_buffer0 == nullptr)
		_blur_buffer0 = _generate_blur_buffer(name() + "_blur0");
	if (_blur_buffer1 == nullptr)
		_blur_buffer1 = _generate_blur_buffer(name() + "_blur1");
	_blur_buffer0->resize(size());
	_blur_buffer1->resize(size());

	static auto	blurShader = GLSL::parse("blur", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/blur.frag");

	auto	totalPass = pass * 4;
	auto	cbuffer = _blur_buffer0;
	auto	ctexture = shared_from_this();
	float	angle = 0;
	std::shared_ptr<Texture>	attachement;
	blurShader->use();
	while (totalPass > 0)
	{
		VEC2			direction;
		direction = mat2_mult_vec2(mat2_rotation(angle), new_vec2(1, 1));
		direction = vec2_scale(direction, radius);
		if (totalPass == 1) {
			attachement = cbuffer->attachement(0);
			cbuffer->set_attachement(0, shared_from_this());
		}
		cbuffer->bind();
		blurShader->set_uniform("in_Direction", direction);
		blurShader->bind_texture("in_Texture_Color", ctexture, GL_TEXTURE0);
		Render::display_quad()->draw();
		angle = CYCLE(angle + (M_PI / 4.f), 0, M_PI);
		if (totalPass == 1)
			cbuffer->set_attachement(0, attachement);
		if (totalPass % 2 == 0) {
			cbuffer = _blur_buffer1;
			ctexture = _blur_buffer0->attachement(0);
		}
		else {
			cbuffer = _blur_buffer0;
			ctexture = _blur_buffer1->attachement(0);
		}
		totalPass--;
	}
	blurShader->use(false);
	Framebuffer::bind_default();
}
