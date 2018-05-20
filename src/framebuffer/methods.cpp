/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/22 21:56:32 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/20 01:39:13 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "Window.hpp"

Framebuffer::Framebuffer(const std::string &name) : Texture(name), _depth(nullptr), _shader(nullptr)
{

}

Framebuffer		*Framebuffer::create(const std::string &name, VEC2 size, Shader &shader,
	int color_attachements, int depth)
{
	Framebuffer	*f;
	int				i;

	f = new Framebuffer(name);
	f->_size = size;
	f->_shader = &shader;
	glGenFramebuffers(1, &f->_glid);
	Engine::add(*f);
	i = -1;
	while (++i < color_attachements) {
		f->create_attachement(GL_RGBA, GL_RGBA16F_ARB);
	}
	if (depth != 0) {
		f->create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	}
	f->setup_attachements();
	return (f);
}

void			Framebuffer::bind(bool to_bind)
{
	if (!to_bind)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Window::size().x, Window::size().y);
		return ;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _glid);
	glViewport(0, 0, size().x, size().y);
	shader().use();
}

void			Framebuffer::bind_default()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::size().x, Window::size().y);
}

Texture			*Framebuffer::create_attachement(GLenum format, GLenum iformat)
{
	std::string tname(std::string("attachement") + std::to_string(_color_attachements.size()));
	bind();
	auto	a = Texture::create(tname, size(), GL_TEXTURE_2D, format, iformat); 
	a->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	a->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	}
	_color_attachements.push_back(a);
	bind(false);
	return (a);
}

void			Framebuffer::setup_attachements()
{
	unsigned		i;
	GLenum			a;
	GLenum			format[2];
	std::vector<GLenum>			color_attachements;

	i = 0;
	while (i < _color_attachements.size())
	{
		attachement(i).format(&format[0],
			&format[1]);
		a = GL_COLOR_ATTACHMENT0 + i;
		if (format[0] != GL_DEPTH_ATTACHMENT) {
			color_attachements.push_back(a);
		}
		i++;
	}
	bind();
	glDrawBuffers(color_attachements.size(), &color_attachements[0]);
	bind(false);
}

void	Framebuffer::_resize_attachement(const int &attachement, const VEC2 &ns)
{
	Texture &t = Framebuffer::attachement(attachement);
	t.resize(ns);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachement,
		GL_TEXTURE_2D, t.glid(), 0);
	t.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	t.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	t.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	t.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	t.set_parameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
}

void	Framebuffer::_resize_depth(const VEC2 &ns)
{
	if (_depth == nullptr) {
		return ;
}
	Texture &t = *_depth;
	/*glDeleteTextures(1, &t._glid);
	glGenTextures(1, &t._glid);
	glBindTexture(t._target, t._glid);
	glTexImage2D(t._target, 0, t._internal_format, ns.x, ns.y, 0,
		t._format, GL_FLOAT, nullptr);
	glBindTexture(t._target, 0);*/
	t.resize(ns);
	t.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	t.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	t.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	t.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	t.set_parameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, t.glid(), 0);
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


Texture			&Framebuffer::attachement(int color_attachement)
{
	return (*_color_attachements[color_attachement]);
}

Texture			&Framebuffer::depth()
{
	return (*_depth);
}

Shader			&Framebuffer::shader()
{
	return (*_shader);
}