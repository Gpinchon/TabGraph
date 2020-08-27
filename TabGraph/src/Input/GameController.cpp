/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:04:43
*/

#include "Input/GameController.hpp"
#include "Input/Events.hpp" // for Events
#include <iostream> // for basic_ostream::operator<<, operator<<, endl
#include <utility> // for pair

Controller::Controller(SDL_JoystickID device)
{
    open(device);
}

Controller::~Controller()
{
    close();
}

bool Controller::is(SDL_JoystickID device)
{
    return (id() == device);
}

bool Controller::is_connected()
{
    return (_is_connected);
}

void Controller::open(SDL_JoystickID device)
{
    if (_is_connected || !SDL_IsGameController(device)) {
        return;
    }
    _gamepad = SDL_GameControllerOpen(device);
    auto j = SDL_GameControllerGetJoystick(_gamepad);
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

void Controller::close()
{
    if (!_is_connected)
        return;
    if (_haptic) {
        SDL_HapticClose(_haptic);
        _haptic = nullptr;
    }
    SDL_GameControllerClose(_gamepad);
    _gamepad = nullptr;
    _instance_id = -1;
    _is_connected = false;
}

void Controller::process_event(SDL_Event* event)
{
    switch (event->type) {
    case SDL_CONTROLLERAXISMOTION:
    case SDL_JOYAXISMOTION: {
        auto callback = _axis_callbacks.find(event->caxis.axis);
        if (callback != _axis_callbacks.end())
            callback->second(&event->caxis);
        break;
    }
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP: {
        auto callback = _button_callbacks.find(event->cbutton.button);
        if (callback != _button_callbacks.end())
            callback->second(&event->cbutton);
        break;
    }
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_JOYDEVICEADDED: {
        std::cout << "Joystick Added " << event->cdevice.which << std::endl;
        open(event->cdevice.which);
        if (_connection_callback != nullptr)
            _connection_callback(&event->cdevice);
        break;
    }
    case SDL_CONTROLLERDEVICEREMOVED:
    case SDL_JOYDEVICEREMOVED: {
        std::cout << "Joystick Removed " << event->cdevice.which << std::endl;
        close();
        if (_disconnect_callback != nullptr)
            _disconnect_callback(&event->cdevice);
        break;
    }
    }
}

void Controller::set_axis_callback(Uint8 type, controller_axis_callback callback)
{
    _axis_callbacks[type] = callback;
}

void Controller::set_button_callback(Uint8 type, controller_button_callback callback)
{
    _button_callbacks[type] = callback;
}

void Controller::set_connection_callback(controller_callback callback)
{
    _connection_callback = callback;
}

void Controller::set_disconnect_callback(controller_callback callback)
{
    _disconnect_callback = callback;
}

void Controller::rumble(float strength, int duration)
{
    SDL_HapticRumblePlay(_haptic, strength, duration);
}

float Controller::axis(SDL_GameControllerAxis axis)
{
    auto value = SDL_GameControllerGetAxis(_gamepad, axis) / 32767.f;
    if (value < 0 && value > -0.1) {
        value = 0;
    } else if (value > 0 && value < 0.1) {
        value = 0;
    }
    return (value);
}

Uint8 Controller::button(SDL_GameControllerButton button)
{
    return (SDL_GameControllerGetButton(_gamepad, button));
}

SDL_JoystickID Controller::id()
{
    return (_instance_id);
}

GameController* GameController::_instance = nullptr;

GameController::GameController()
{
    Events::Add(this, SDL_CONTROLLERAXISMOTION);
    Events::Add(this, SDL_JOYAXISMOTION);
    Events::Add(this, SDL_CONTROLLERBUTTONDOWN);
    Events::Add(this, SDL_CONTROLLERBUTTONUP);
    Events::Add(this, SDL_JOYBUTTONDOWN);
    Events::Add(this, SDL_JOYBUTTONUP);
    Events::Add(this, SDL_CONTROLLERDEVICEADDED);
    Events::Add(this, SDL_JOYDEVICEADDED);
    Events::Add(this, SDL_CONTROLLERDEVICEREMOVED);
    Events::Add(this, SDL_JOYDEVICEREMOVED);
}

GameController* GameController::_get()
{
    if (_instance == nullptr)
        _instance = new GameController();
    return (_instance);
}

void GameController::process_event(SDL_Event* event)
{
    auto controllerIndex = -1;
    if (event->type == SDL_CONTROLLERDEVICEADDED || event->type == SDL_JOYDEVICEADDED) {
        controllerIndex = event->cdevice.which;
    } else {
        controllerIndex = get_controller_index(event->cdevice.which);
    }
    if (controllerIndex != -1)
        Get(controllerIndex)->process_event(event);
}

Controller* GameController::Get(SDL_JoystickID device)
{
    auto controller = _get()->_controllers.find(device);
    if (controller == _get()->_controllers.end()) {
        auto newcontroller = new Controller(device);
        _get()->_controllers[device] = newcontroller;
        return (newcontroller);
    }
    return (controller->second);
}

void GameController::remove(SDL_JoystickID i)
{
    _get()->_controllers.erase(i);
}

int GameController::get_controller_index(SDL_JoystickID device)
{
    for (auto controller : _get()->_controllers) {
        if (controller.second->is(device))
            return (controller.first);
    }
    return (-1);
}
