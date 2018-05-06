/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:22:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/01 21:04:36 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"

Window *g_window = nullptr;

/*
** window is a singleton
*/

Window	&Window::_get(void)
{
	if (!g_window)
		g_window = new Window();
	return (*g_window);
}

void		Window::swap()
{
	SDL_GL_SwapWindow(_get()._sdl_window);
}

void		Window::init(const std::string &name, int width, int height)
{
	_get()._sdl_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	_get()._gl_context = SDL_GL_CreateContext(_get()._sdl_window);
	_get()._clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	glewExperimental = GL_TRUE;
	if (!_get()._sdl_window || glewInit() != GLEW_OK)
		return ;
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	Shader::load("render",
		"./res/shaders/render.vert", "./res/shaders/render.frag");
	Shader::load("shadow",
		"./res/shaders/shadow.vert", "./res/shaders/shadow.frag");
	Shader::load("blur",
		"./res/shaders/blur.vert", "./res/shaders/blur.frag");
	std::cout << "Window::init " << Shader::get_by_name("render") << std::endl;
	_get()._render_buffer = &Framebuffer::create(
		"window_render_buffer", vec2_scale(_get().size(),
		Engine::internal_quality()), *Shader::get_by_name("render"), 0, 0);
	_get().render_buffer().create_attachement(GL_RGBA, GL_RGBA16F_ARB);
	_get().render_buffer().create_attachement(GL_RGB, GL_RGB16F_ARB);
	_get().render_buffer().create_attachement(GL_RGB, GL_RGB16F_ARB);
	_get().render_buffer().create_attachement(GL_RGB, GL_RGB32F_ARB);
	_get().render_buffer().create_attachement(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24);
	_get().render_buffer().setup_attachements();
}

GLbitfield	&Window::clear_mask()
{
	return (_get()._clear_mask);
}

VEC2		Window::size(void)
{
	int	w;
	int	h;

	SDL_GetWindowSize(_get()._sdl_window, &w, &h);
	return (new_vec2(w, h));
}

void		Window::resize(void)
{
	VEC2		size;
	static VEC2	lastSize = new_vec2(0, 0);

	size = vec2_scale(Window::size(),
		Engine::internal_quality());
	if (lastSize.x != size.x || lastSize.y != size.y)
	{
		Window::render_buffer().resize(size);
		lastSize = size;
	}
}

void		Window::fullscreen(const bool &fullscreen)
{
	SDL_SetWindowFullscreen(_get()._sdl_window,
		fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	Window::resize();
}

Framebuffer	&Window::render_buffer()
{
	return (*_get()._render_buffer);
}