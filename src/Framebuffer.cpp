/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Framebuffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/22 21:56:32 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/09 17:33:28 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "Window.hpp"

Attachement::Attachement(const std::string &name) : Texture(name){}
Attachement::Attachement(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format) : Texture(name, s, target, f, fi, data_format) {};

GLenum	get_data_format(GLenum internal_format)
{
	switch (internal_format) {
		case GL_R8_SNORM :
		case GL_RG8_SNORM :
		case GL_RGB8_SNORM :
		case GL_RGBA8_SNORM :
		case GL_SRGB8 :
			return (GL_BYTE);
		case GL_R16F :
		case GL_RG16F :
		case GL_RGB16F :
		case GL_RGBA16F :
			return (GL_HALF_FLOAT);
		case GL_R32F :
		case GL_RG32F :
		case GL_RGB32F :
		case GL_RGBA32F :
			return (GL_FLOAT);
		case GL_R11F_G11F_B10F :
			return (GL_UNSIGNED_INT_10F_11F_11F_REV);
		case GL_R16 :
		case GL_RG16 :
		case GL_RGB16 :
			return (GL_UNSIGNED_SHORT);
		case GL_R16_SNORM :
		case GL_RG16_SNORM :
		case GL_RGB16_SNORM :
			return (GL_SHORT);
		default : return (GL_UNSIGNED_BYTE);
	}
}

Attachement		*Attachement::create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi)
{
	Attachement	*t;

	t = new Attachement(name, s, target, f, fi, get_data_format(fi));
	glBindTexture(t->_target, t->_glid);
	if (s.x > 0 && s.y > 0) {
		glTexImage2D(t->_target, 0, fi, s.x, s.y, 0, f, GL_FLOAT, nullptr);
	}
	glBindTexture(t->_target, 0);
	Engine::add(*t);
#ifdef GL_DEBUG
	glObjectLabel(GL_RENDERBUFFER, t->_glid, -1, name.c_str());
#endif //GL_DEBUG
	return (t);
}

bool		Attachement::is_loaded() {
	return (true);
}

void		Attachement::load() {
}

void		Attachement::unload() {
}

Framebuffer::Framebuffer(const std::string &name) : Texture(name) {}

Framebuffer		*Framebuffer::create(const std::string &name, VEC2 size, Shader *shader,
	int color_attachements, int depth)
{
	Framebuffer	*f;
	int				i;

	f = new Framebuffer(name);
	f->_size = size;
	f->_shader = shader;
	glGenFramebuffers(1, &f->_glid);
	Engine::add(*f);
	i = 0;
	while (i < color_attachements) {
		f->create_attachement(GL_RGBA, GL_RGBA16F);
		i++;
	}
	if (depth != 0) {
		f->create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	}
	f->setup_attachements();
#ifdef GL_DEBUG
	glObjectLabel(GL_FRAMEBUFFER, f->_glid, -1, name.c_str());
#endif //GL_DEBUG
	return (f);
}

bool			Framebuffer::is_loaded()
{
	return (true);
}

void			Framebuffer::load()
{
	return;
}

void			Framebuffer::bind(bool to_bind)
{
	if (!to_bind)
	{
		bind_default();
		if (shader() != nullptr)
			shader()->use(false);
		return ;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _glid);
	glViewport(0, 0, size().x, size().y);
	if (shader() == nullptr)
		return ;
	shader()->use();
}

void			Framebuffer::bind_default()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::size().x, Window::size().y);
}

Texture			*Framebuffer::create_attachement(GLenum format, GLenum iformat)
{
	std::string tname;
	if (format == GL_DEPTH_COMPONENT)
		tname = (name() + "_depth");
	else
		tname = (name() + "_attachement_" + std::to_string(_color_attachements.size()));
	bind();
	auto	a = Attachement::create(tname, size(), GL_TEXTURE_2D, format, iformat); 
	a->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	a->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (format == GL_DEPTH_COMPONENT)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, a->glid(), 0);
		_depth = a;
	}
	else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0
			+ _color_attachements.size(), GL_TEXTURE_2D,
			a->glid(), 0);
		_color_attachements.push_back(a);
	}
	bind(false);
	return (a);
}

void			Framebuffer::setup_attachements()
{
	unsigned		i;
	GLenum			format[2];
	std::vector<GLenum>			color_attachements;

	i = 0;
	while (i < _color_attachements.size())
	{
		attachement(i)->format(&format[0], &format[1]);
		if (format[0] != GL_DEPTH_COMPONENT) {
			color_attachements.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		i++;
	}
	bind();
	glDrawBuffers(color_attachements.size(), &color_attachements[0]);
	bind(false);
}

void	Framebuffer::_resize_attachement(const int &attachement, const VEC2 &ns)
{
	auto	t = Framebuffer::attachement(attachement);
	t->resize(ns);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachement,
		GL_TEXTURE_2D, t->glid(), 0);
	t->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	t->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	t->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	t->set_parameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
}

void	Framebuffer::_resize_depth(const VEC2 &ns)
{
	if (_depth == nullptr) {
		return ;
	}
	_depth->resize(ns);
	_depth->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	_depth->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	_depth->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_depth->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_depth->set_parameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, _depth->glid(), 0);
}

void		Framebuffer::resize(const VEC2 &new_size)
{
	unsigned		i;

	if (size().x == new_size.x && size().y == new_size.y) {
		return ;
	}
	bind();
	_size = new_size;
	i = 0;
	while (i < _color_attachements.size())
	{
		_resize_attachement(i, new_size);
		i++;
	}
	_resize_depth(new_size);
	bind(false);
}

void		Framebuffer::set_shader(Shader *shader)
{
	_shader = shader;
}

void		Framebuffer::set_attachement(unsigned color_attachement, Texture *texture)
{
	if (color_attachement >= _color_attachements.size())
		throw std::runtime_error(name() + " : Color attachement index is out of bound");
	_color_attachements[color_attachement] = texture;
	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_attachement, texture->target(), texture->glid(), 0);
	bind(false);
}

Texture			*Framebuffer::attachement(unsigned color_attachement)
{
	if (unsigned(color_attachement) >= _color_attachements.size())
		return (nullptr);
	return (_color_attachements[color_attachement]);
}

Texture			*Framebuffer::depth()
{
	return (_depth);
}

Shader			*Framebuffer::shader()
{
	return (_shader);
}
