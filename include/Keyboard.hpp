/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Keyboard.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/05 21:13:24 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 21:25:51 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "InputDevice.hpp"
#include "Events.hpp"

class Keyboard : InputDevice
{
public:
	static void	set_callback(SDL_Scancode key, t_callback callback);
	void		process_event(SDL_Event *);
private:
	static Keyboard	&_get();
	static Keyboard	*_instance;
	std::map<SDL_Scancode, t_callback>	_callbacks;
	Keyboard();
};