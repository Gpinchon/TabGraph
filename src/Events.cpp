/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Events.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/16 17:24:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 21:36:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Events.hpp"
#include "Window.hpp"
#include "InputDevice.hpp"

#include <iostream>

void	print_event_name(SDL_Event *event)
{
	switch (event->type) {
		case SDL_FIRSTEVENT: {
			std::cout << "Event : SDL_FIRSTEVENT" << std::endl;
			break;
		}
		case SDL_QUIT: {
			std::cout << "Event : SDL_QUIT" << std::endl;
			break;
		}
		case SDL_APP_TERMINATING: {
			std::cout << "Event : SDL_APP_TERMINATING" << std::endl;
			break;
		}
		case SDL_APP_LOWMEMORY: {
			std::cout << "Event : SDL_APP_LOWMEMORY" << std::endl;
			break;
		}
		case SDL_APP_WILLENTERBACKGROUND: {
			std::cout << "Event : SDL_APP_WILLENTERBACKGROUND" << std::endl;
			break;
		}
		case SDL_APP_DIDENTERBACKGROUND: {
			std::cout << "Event : SDL_APP_DIDENTERBACKGROUND" << std::endl;
			break;
		}
		case SDL_APP_WILLENTERFOREGROUND: {
			std::cout << "Event : SDL_APP_WILLENTERFOREGROUND" << std::endl;
			break;
		}
		case SDL_APP_DIDENTERFOREGROUND: {
			std::cout << "Event : SDL_APP_DIDENTERFOREGROUND" << std::endl;
			break;
		}
		case SDL_WINDOWEVENT: {
			std::cout << "Event : SDL_WINDOWEVENT" << std::endl;
			break;
		}
		case SDL_SYSWMEVENT: {
			std::cout << "Event : SDL_SYSWMEVENT" << std::endl;
			break;
		}
		case SDL_KEYDOWN: {
			std::cout << "Event : SDL_KEYDOWN" << std::endl;
			break;
		}
		case SDL_KEYUP: {
			std::cout << "Event : SDL_KEYUP" << std::endl;
			break;
		}
		case SDL_TEXTEDITING: {
			std::cout << "Event : SDL_TEXTEDITING" << std::endl;
			break;
		}
		case SDL_TEXTINPUT: {
			std::cout << "Event : SDL_TEXTINPUT" << std::endl;
			break;
		}
		case SDL_KEYMAPCHANGED: {
			std::cout << "Event : SDL_KEYMAPCHANGED" << std::endl;
			break;
		}
		case SDL_MOUSEMOTION: {
			std::cout << "Event : SDL_MOUSEMOTION" << std::endl;
			break;
		}
		case SDL_MOUSEBUTTONDOWN: {
			std::cout << "Event : SDL_MOUSEBUTTONDOWN" << std::endl;
			break;
		}
		case SDL_MOUSEBUTTONUP: {
			std::cout << "Event : SDL_MOUSEBUTTONUP" << std::endl;
			break;
		}
		case SDL_MOUSEWHEEL: {
			std::cout << "Event : SDL_MOUSEWHEEL" << std::endl;
			break;
		}
		case SDL_JOYAXISMOTION: {
			std::cout << "Event : SDL_JOYAXISMOTION" << std::endl;
			break;
		}
		case SDL_JOYBALLMOTION: {
			std::cout << "Event : SDL_JOYBALLMOTION" << std::endl;
			break;
		}
		case SDL_JOYHATMOTION: {
			std::cout << "Event : SDL_JOYHATMOTION" << std::endl;
			break;
		}
		case SDL_JOYBUTTONDOWN: {
			std::cout << "Event : SDL_JOYBUTTONDOWN" << std::endl;
			break;
		}
		case SDL_JOYBUTTONUP: {
			std::cout << "Event : SDL_JOYBUTTONUP" << std::endl;
			break;
		}
		case SDL_JOYDEVICEADDED: {
			std::cout << "Event : SDL_JOYDEVICEADDED" << std::endl;
			break;
		}
		case SDL_JOYDEVICEREMOVED: {
			std::cout << "Event : SDL_JOYDEVICEREMOVED" << std::endl;
			break;
		}
		case SDL_CONTROLLERAXISMOTION: {
			std::cout << "Event : SDL_CONTROLLERAXISMOTION" << std::endl;
			break;
		}
		case SDL_CONTROLLERBUTTONDOWN: {
			std::cout << "Event : SDL_CONTROLLERBUTTONDOWN" << std::endl;
			break;
		}
		case SDL_CONTROLLERBUTTONUP: {
			std::cout << "Event : SDL_CONTROLLERBUTTONUP" << std::endl;
			break;
		}
		case SDL_CONTROLLERDEVICEADDED: {
			std::cout << "Event : SDL_CONTROLLERDEVICEADDED" << std::endl;
			break;
		}
		case SDL_CONTROLLERDEVICEREMOVED: {
			std::cout << "Event : SDL_CONTROLLERDEVICEREMOVED" << std::endl;
			break;
		}
		case SDL_CONTROLLERDEVICEREMAPPED: {
			std::cout << "Event : SDL_CONTROLLERDEVICEREMAPPED" << std::endl;
			break;
		}
		case SDL_FINGERDOWN: {
			std::cout << "Event : SDL_FINGERDOWN" << std::endl;
			break;
		}
		case SDL_FINGERUP: {
			std::cout << "Event : SDL_FINGERUP" << std::endl;
			break;
		}
		case SDL_FINGERMOTION: {
			std::cout << "Event : SDL_FINGERMOTION" << std::endl;
			break;
		}
		case SDL_DOLLARGESTURE: {
			std::cout << "Event : SDL_DOLLARGESTURE" << std::endl;
			break;
		}
		case SDL_DOLLARRECORD: {
			std::cout << "Event : SDL_DOLLARRECORD" << std::endl;
			break;
		}
		case SDL_MULTIGESTURE: {
			std::cout << "Event : SDL_MULTIGESTURE" << std::endl;
			break;
		}
		case SDL_CLIPBOARDUPDATE: {
			std::cout << "Event : SDL_CLIPBOARDUPDATE" << std::endl;
			break;
		}
		case SDL_DROPFILE: {
			std::cout << "Event : SDL_DROPFILE" << std::endl;
			break;
		}
		case SDL_DROPTEXT: {
			std::cout << "Event : SDL_DROPTEXT" << std::endl;
			break;
		}
		case SDL_DROPBEGIN: {
			std::cout << "Event : SDL_DROPBEGIN" << std::endl;
			break;
		}
		case SDL_DROPCOMPLETE: {
			std::cout << "Event : SDL_DROPCOMPLETE" << std::endl;
			break;
		}
		case SDL_AUDIODEVICEADDED: {
			std::cout << "Event : SDL_AUDIODEVICEADDED" << std::endl;
			break;
		}
		case SDL_AUDIODEVICEREMOVED: {
			std::cout << "Event : SDL_AUDIODEVICEREMOVED" << std::endl;
			break;
		}
		case SDL_RENDER_TARGETS_RESET: {
			std::cout << "Event : SDL_RENDER_TARGETS_RESET" << std::endl;
			break;
		}
		case SDL_RENDER_DEVICE_RESET: {
			std::cout << "Event : SDL_RENDER_DEVICE_RESET" << std::endl;
			break;
		}
		case SDL_USEREVENT: {
			std::cout << "Event : SDL_USEREVENT" << std::endl;
			break;
		}
		case SDL_LASTEVENT: {
			std::cout << "Event : SDL_LASTEVENT" << std::endl;
			break;
		}
	}
}

Events	*Events::_instance = nullptr;

Events::Events() {
}

Events	&Events::_get()
{
	if (_instance == nullptr)
		_instance = new Events();
	return (*_instance);
}

void	Events::add(InputDevice *device, SDL_EventType event_type)
{
	_get()._input_devices[event_type] = device;
}

void	Events::set_refresh_callback(t_callback callback)
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

int		Events::filter(void */*unused*/, SDL_Event *event)
{
	if (event->type == SDL_QUIT) {
		Engine::stop();
	}
	else if (event->type == SDL_WINDOWEVENT) {
		Events::window(event);
	}
	auto	inputdevice = _get()._input_devices.find(event->type);
	if (inputdevice != _get()._input_devices.end() && inputdevice->second != nullptr) {
		inputdevice->second->process_event(event);
	}
	/*
	else if (event->type == SDL_KEYUP || event->type == SDL_KEYDOWN) {
		Keyboard::process_event(event);
	}
	else if (
		event->type == SDL_CONTROLLERAXISMOTION ||
		event->type == SDL_JOYAXISMOTION ||
		event->type == SDL_CONTROLLERBUTTONDOWN ||
		event->type == SDL_CONTROLLERBUTTONUP ||
		event->type == SDL_JOYBUTTONDOWN ||
		event->type == SDL_JOYBUTTONUP ||
		event->type == SDL_CONTROLLERDEVICEADDED ||
		event->type == SDL_JOYDEVICEADDED ||
		event->type == SDL_CONTROLLERDEVICEREMOVED ||
		event->type == SDL_JOYDEVICEREMOVED) {
		GameController::process_event(event);
	}*/
	return (0);
}

int		Events::refresh()
{
	if (_get()._rcallback != nullptr) {
		_get()._rcallback(nullptr);
	}
	return (0);
}
