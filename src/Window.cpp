/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:22:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/18 18:04:56 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Framebuffer.hpp"
#include "Events.hpp"
#include "Window.hpp"
#include "Errors.hpp"
#include <unistd.h>

Window *Window::_instance = nullptr;

Window::Window()
{
	Events::add(this, SDL_WINDOWEVENT);
}

void	Window::process_event(SDL_Event *event)
{
	if (event->window.event == SDL_WINDOWEVENT_CLOSE) {
		Engine::stop();
	}
	else if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
		//Window::resize();
	}
}

/*
** window is a singleton
*/

Window	&Window::_get()
{
	if (_instance == nullptr)
		_instance = new Window();
	return (*_instance);
}

void		Window::swap()
{
	SDL_GL_SwapWindow(_get()._sdl_window);
}

void		Window::init(const std::string &name, int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_GameControllerEventState(SDL_ENABLE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, CFG::Msaa());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	if (nullptr == _get()._sdl_window)
		_get()._sdl_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, width, height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
			SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	if (_get()._sdl_window == nullptr) {
		throw std::runtime_error(SDL_GetError());
	}
	_get()._gl_context = SDL_GL_CreateContext(_get()._sdl_window);
	if (_get()._gl_context == nullptr) {
		throw std::runtime_error(SDL_GetError());
	}
	_get()._clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	glewExperimental = GL_TRUE;
	auto error = glewInit();
	if (error != GLEW_OK) {
		throw std::runtime_error(reinterpret_cast<const char*>(glewGetErrorString(error)));
	}
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#ifdef GL_DEBUG
	glCheckError();
#endif //GL_DEBUG
}

GLbitfield	&Window::clear_mask()
{
	return (_get()._clear_mask);
}

VEC2		Window::internal_resolution()
{
	return (vec2_scale(Window::size(), Engine::internal_quality()));
}

VEC2		Window::size()
{
	int	w;
	int	h;

	SDL_GL_GetDrawableSize(_get()._sdl_window, &w, &h);
	return (new_vec2(w, h));
}

void		Window::resize(const VEC2 &size)
{
	SDL_SetWindowSize(_get()._sdl_window, size.x, size.y);
}

void		Window::fullscreen(const bool &fullscreen)
{
	SDL_SetWindowFullscreen(_get()._sdl_window,
		fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}
