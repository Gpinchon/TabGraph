/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GameController.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/05 20:12:19 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/06 17:06:38 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GameController.hpp"

Controller::Controller(SDL_JoystickID device)
{
	open(device);
}

Controller::~Controller()
{
	close();
}
#include <iostream>

bool	Controller::is(SDL_JoystickID device)
{
	return (id() == device);
}

bool	Controller::is_connected()
{
	return (_is_connected);
}

void	Controller::open(SDL_JoystickID device)
{
	if (_is_connected || !SDL_IsGameController(device)) {
		return ;
	}
	_gamepad = SDL_GameControllerOpen(device);
	auto	j = SDL_GameControllerGetJoystick(_gamepad);
	_instance_id = SDL_JoystickInstanceID(j);
	_is_connected = true;
	if (SDL_JoystickIsHaptic(j)) {
		_haptic = SDL_HapticOpenFromJoystick(j);
		if (SDL_HapticRumbleSupported(_haptic)) {
			SDL_HapticRumbleInit(_haptic);
		}
	}
	std::cout << "Joystick Instance " << _instance_id << std::endl;
}

void	Controller::close()
{
	if (!_is_connected)
		return ;
	if (_haptic)
	{
		SDL_HapticClose(_haptic);
		_haptic = nullptr;
	}
	SDL_GameControllerClose(_gamepad);
	_gamepad = nullptr;
	_instance_id = -1;
	_is_connected = false;
}

void	Controller::process_event(SDL_Event *event)
{
	switch (event->type) {
		case SDL_CONTROLLERAXISMOTION:
		case SDL_JOYAXISMOTION: {
			auto	callback = _axis_callbacks.find(event->caxis.axis);
			if (callback != _axis_callbacks.end())
				callback->second(event);
			break;
		}
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP: {
			auto	callback = _button_callbacks.find(event->cbutton.button);
			if (callback != _button_callbacks.end())
				callback->second(event);
			break;
		}
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_JOYDEVICEADDED: {
			std::cout << "Joystick Added " << event->cdevice.which << std::endl;
			open(event->cdevice.which);
			if (_connection_callback != nullptr)
				_connection_callback(event);
			break;
		}
		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_JOYDEVICEREMOVED: {
			std::cout << "Joystick Removed " << event->cdevice.which << std::endl;
			close();
			if (_disconnect_callback != nullptr)
				_disconnect_callback(event);
			break;
		}
	}
}

void	Controller::set_axis_callback(Uint8 type, t_callback callback)
{
	_axis_callbacks[type] = callback;
}

void	Controller::set_button_callback(Uint8 type, t_callback callback)
{
	_button_callbacks[type] = callback;
}

void			Controller::set_connection_callback(t_callback callback)
{
	_connection_callback = callback;
}

void			Controller::set_disconnect_callback(t_callback callback)
{
	_disconnect_callback = callback;
}

void	Controller::rumble(float strength, int duration)
{
	SDL_HapticRumblePlay(_haptic, strength, duration);
}

float			Controller::axis(SDL_GameControllerAxis axis)
{
	auto	value = SDL_GameControllerGetAxis(_gamepad, axis) / 32767.f;
	if (value < 0 && value > -0.1) {
		value = 0;
	}
	else if (value > 0 && value < 0.1) {
		value = 0;
	}
	return (value);
}

Uint8			Controller::button(SDL_GameControllerButton button)
{
	return (SDL_GameControllerGetButton(_gamepad, button));
}

SDL_JoystickID	Controller::id()
{
	return (_instance_id);
}

GameController	*GameController::_instance = new GameController();

GameController::GameController()
{
	Events::add(this, SDL_CONTROLLERAXISMOTION);
	Events::add(this, SDL_JOYAXISMOTION);
	Events::add(this, SDL_CONTROLLERBUTTONDOWN);
	Events::add(this, SDL_CONTROLLERBUTTONUP);
	Events::add(this, SDL_JOYBUTTONDOWN);
	Events::add(this, SDL_JOYBUTTONUP);
	Events::add(this, SDL_CONTROLLERDEVICEADDED);
	Events::add(this, SDL_JOYDEVICEADDED);
	Events::add(this, SDL_CONTROLLERDEVICEREMOVED);
	Events::add(this, SDL_JOYDEVICEREMOVED);
}

GameController	*GameController::_get()
{
	return (_instance);
}

void	GameController::process_event(SDL_Event *event)
{
	auto	controllerIndex = -1;
	if (event->type == SDL_CONTROLLERDEVICEADDED || event->type == SDL_JOYDEVICEADDED) {
		controllerIndex = event->cdevice.which;
	}
	else {
		controllerIndex = get_controller_index(event->cdevice.which);
	}
	if (controllerIndex != -1)
		get(controllerIndex)->process_event(event);
}

Controller	*GameController::get(SDL_JoystickID device)
{
	auto	controller = _get()->_controllers.find(device);
	if (controller == _get()->_controllers.end())
	{
		auto	newcontroller = new Controller(device);
		_get()->_controllers[device] = newcontroller;
		return (newcontroller);
	}
	return (controller->second);
}

void	GameController::remove(SDL_JoystickID i)
{
	_get()->_controllers.erase(i);
}

int		GameController::get_controller_index(SDL_JoystickID device)
{
	for (auto controller : _get()->_controllers)
	{
		if (controller.second->is(device))
			return (controller.first);
	}
	return (-1);
}
