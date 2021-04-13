/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:30
*/

#pragma once

#include "Event/Signal.hpp"
#include "InputDevice.hpp" // for InputDevice

#include <SDL_events.h> // for SDL_Event, SDL_ControllerAxisEvent
#include <SDL_gamecontroller.h> // for SDL_GameController, SDL_GameCon...
#include <SDL_haptic.h> // for SDL_Haptic
#include <SDL_joystick.h> // for SDL_JoystickID
#include <SDL_stdinc.h> // for Uint8
#include <array> // for array
#include <map> // for map
#include <memory>

//TODO : add default button mapping and button repeat for gamepad and other InputDevice

class Controller : InputDevice {
public:
    Controller() = delete;
    Controller(SDL_JoystickID id);
    ~Controller();
    bool is(SDL_JoystickID device);
    bool is_connected();
    void open(SDL_JoystickID device);
    void close();
    void process_event(SDL_Event* event) override;
    Signal<const SDL_ControllerAxisEvent&>& OnAxis(const SDL_GameControllerAxis&);
    Signal<const SDL_ControllerButtonEvent&>& OnButton(const SDL_GameControllerButton&);
    Signal<const SDL_ControllerButtonEvent&>& OnButtonUp(const SDL_GameControllerButton&);
    Signal<const SDL_ControllerButtonEvent&>& OnButtonDown(const SDL_GameControllerButton&);
    Signal<const SDL_ControllerDeviceEvent&>& OnConnection();
    Signal<const SDL_ControllerDeviceEvent&>& OnDisconnection();
    void rumble(float strength, int duration);
    SDL_JoystickID id();
    float axis(SDL_GameControllerAxis);
    Uint8 button(SDL_GameControllerButton);

private:
    SDL_GameController* _gamepad { nullptr };
    SDL_Haptic* _haptic { nullptr };
    SDL_JoystickID _instanceId { -1 };
    bool _is_connected { false };
    Signal<const SDL_ControllerDeviceEvent&> _onConnection;
    Signal<const SDL_ControllerDeviceEvent&> _onDisconnection;
    std::array<Signal<const SDL_ControllerAxisEvent&>, SDL_CONTROLLER_AXIS_MAX> _onAxis;
    std::array<Signal<const SDL_ControllerButtonEvent&>, SDL_CONTROLLER_BUTTON_MAX> _onButton;
    std::array<Signal<const SDL_ControllerButtonEvent&>, SDL_CONTROLLER_BUTTON_MAX> _onButtonUp;
    std::array<Signal<const SDL_ControllerButtonEvent&>, SDL_CONTROLLER_BUTTON_MAX> _onButtonDown;
};

class GameController : InputDevice {
public:
    static Controller& Get(SDL_JoystickID index);
    static void remove(SDL_JoystickID index);
    virtual void process_event(SDL_Event* event) override;

private:
    int get_controller_index(SDL_JoystickID device);
    std::map<SDL_JoystickID, std::unique_ptr<Controller>> _controllers;
    static GameController& _get();
    GameController();
};