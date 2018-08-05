/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GameController.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/05 20:13:01 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 20:22:50 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "InputDevice.hpp"
#include "Events.hpp"
#include <map>

class	Controller : InputDevice
{
public:
	Controller(SDL_JoystickID id);
	~Controller();
	SDL_JoystickID	id();
	void			process_event(SDL_Event *event);
	void			set_axis_callback(Uint8 type, t_callback callback);
	void			set_button_callback(Uint8 type, t_callback callback);
private:
	SDL_GameController				*_gamepad{nullptr};
	SDL_Haptic						*_haptic{nullptr};
	SDL_JoystickID					_instance_id{-1};
	bool							_is_connected{false};
	std::map<Uint8, t_callback>		_axis_callbacks;
	std::map<Uint8, t_callback>		_button_callbacks;
	Controller();
};

class	GameController : InputDevice
{
public:
	static Controller	*open(SDL_JoystickID device);
	static Controller	*get(SDL_JoystickID device);
	static void			remove(SDL_JoystickID device);
	virtual void		process_event(SDL_Event *event);
private:
	std::map<SDL_JoystickID, Controller*>	_controllers;
	static GameController	*_get();
	static GameController	*_instance;
	GameController();
};