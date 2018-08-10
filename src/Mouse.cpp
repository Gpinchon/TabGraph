/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mouse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/07 18:14:54 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/07 19:30:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mouse.hpp"
#include "Events.hpp"

Mouse	*Mouse::_instance = nullptr;

Mouse::Mouse()
{
	Events::add(this, SDL_MOUSEMOTION);
	Events::add(this, SDL_MOUSEBUTTONDOWN);
	Events::add(this, SDL_MOUSEBUTTONUP);
	Events::add(this, SDL_MOUSEWHEEL);
}

void	Mouse::process_event(SDL_Event *event)
{
	switch (event->type)
	{
		case SDL_MOUSEMOTION: {
			if (_get()->_move_callback != nullptr)
				_get()->_move_callback(&event->motion);
			break;
		}
		case SDL_MOUSEBUTTONDOWN :
		case SDL_MOUSEBUTTONUP : {
			if (_get()->_button_callbacks[event->button.button] != nullptr)
				_get()->_button_callbacks[event->button.button](&event->button);
			break;
		}
		case SDL_MOUSEWHEEL : {
			if (_get()->_wheel_callback != nullptr)
				_get()->_wheel_callback(&event->wheel);
			break;
		}
	}
}

bool	Mouse::set_relative(SDL_bool value)
{
	SDL_SetRelativeMouseMode(value);
	return (SDL_GetRelativeMouseMode());
}

void	Mouse::set_move_callback(mouse_motion_callback callback)
{
	_get()->_move_callback = callback;
}

void	Mouse::set_wheel_callback(mouse_wheel_callback callback)
{
	_get()->_wheel_callback = callback;
}

void	Mouse::set_button_callback(mouse_button_callback callback, Uint8 button)
{
	_get()->_button_callbacks[button] = callback;
}

bool	Mouse::button(Uint8 button)
{
	auto	mask = SDL_GetMouseState(nullptr, nullptr);
	return (mask & SDL_BUTTON(button));
}

void	Mouse::position(Sint32 &x, Sint32 &y)
{
	SDL_GetMouseState(&x, &y);
}

Mouse	*Mouse::_get()
{
	if (_instance == nullptr)
		_instance = new Mouse();
	return (_instance);
}