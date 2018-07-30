/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/16 17:24:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 23:42:30 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Events.hpp"
#include "Window.hpp"

Events	*Events::_instance = new Events();

Events	&Events::_get()
{
	return (*_instance);
}

void	Events::set_key_callback(SDL_Scancode key, t_kcallback callback)
{
	_get()._kcallbacks.at(key) = callback;
}

void	Events::set_refresh_callback(t_kcallback callback)
{
	_get()._rcallback = callback;
}


void	Events::window(SDL_Event *event)
{
	if (event->window.event == SDL_WINDOWEVENT_CLOSE) {
		Engine::stop();
	}
	else if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
		Window::resize();
	}
}

void	Events::keyboard(SDL_Event *event)
{
	if (_get()._kcallbacks.at(event->key.keysym.scancode) != nullptr) {
		_get()._kcallbacks.at(event->key.keysym.scancode)(event);
	}
}

int		Events::filter(void */*unused*/, SDL_Event *event)
{
	if (event->type == SDL_QUIT) {
		Engine::stop();
	}
	else if (event->type == SDL_WINDOWEVENT) {
		Events::window(event);
	}
	else if (event->type == SDL_KEYUP || event->type == SDL_KEYDOWN) {
		Events::keyboard(event);
	}
	return (0);
}

int		Events::refresh()
{
	if (_get()._rcallback != nullptr) {
		_get()._rcallback(nullptr);
	}
	return (0);
}
