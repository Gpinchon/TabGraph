/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Keyboard.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/05 21:13:24 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/07 19:31:33 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "InputDevice.hpp"
#include "Events.hpp"

typedef void	(*keyboard_callback)(SDL_KeyboardEvent *event);

class Keyboard : InputDevice
{
public:
	static Uint8	key(SDL_Scancode);
	static void		set_callback(SDL_Scancode key, keyboard_callback callback);
	void			process_event(SDL_Event *);
private:
	static Keyboard	&_get();
	static Keyboard	*_instance;
	std::map<SDL_Scancode, keyboard_callback>	_callbacks;
	Keyboard();
};