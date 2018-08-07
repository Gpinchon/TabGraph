/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Keyboard.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/05 21:15:02 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/07 19:32:00 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Keyboard.hpp"

Keyboard	*Keyboard::_instance = new Keyboard();

Keyboard::Keyboard()
{
	Events::add(this, SDL_KEYUP);
	Events::add(this, SDL_KEYDOWN);
}

Keyboard	&Keyboard::_get()
{
	return (*_instance);
}

Uint8		Keyboard::key(SDL_Scancode key)
{
	return (SDL_GetKeyboardState(nullptr)[key]);
}

void	Keyboard::set_callback(SDL_Scancode key, keyboard_callback callback)
{
	_get()._callbacks[key] = callback;
}

void	Keyboard::process_event(SDL_Event *event)
{
	auto	callback = _get()._callbacks.find(event->key.keysym.scancode);
	if (callback != _get()._callbacks.end())
		callback->second(&event->key);
}
