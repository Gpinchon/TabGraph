/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GameController.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/05 20:12:19 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/06 00:49:10 by gpinchon         ###   ########.fr       */
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
void	Controller::open(SDL_JoystickID device)
{
	if (!SDL_IsGameController(device)) {
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
}

bool	Controller::is_connected()
{
	return (_is_connected);
}

void	Controller::close()
{
	if (_is_connected)
	{
		_is_connected = false;
		if (_haptic)
		{
			SDL_HapticClose(_haptic);
			_haptic = nullptr;
		}
		SDL_GameControllerClose(_gamepad);
		_gamepad = nullptr;
	}
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
		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_JOYDEVICEREMOVED: {
			std::cout << "Joystick Removed " << event->cdevice.which << std::endl;
			close();
			break;
		}
		case SDL_CONTROLLERDEVICEADDED:
		case SDL_JOYDEVICEADDED: {
			std::cout << "Joystick Added " << event->cdevice.which << std::endl;
			open(event->cdevice.which);
			break;
		}
	}
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

void	Controller::set_axis_callback(Uint8 type, t_callback callback)
{
	_axis_callbacks[type] = callback;
}

void	Controller::set_button_callback(Uint8 type, t_callback callback)
{
	_button_callbacks[type] = callback;
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
	get(event->caxis.which)->process_event(event);
}