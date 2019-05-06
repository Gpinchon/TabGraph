/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:28:08
*/

#pragma once

#include "Events.hpp"
#include "InputDevice.hpp"
#include <map>

typedef void (*controller_axis_callback)(SDL_ControllerAxisEvent* event);
typedef void (*controller_button_callback)(SDL_ControllerButtonEvent* event);
typedef void (*controller_callback)(SDL_ControllerDeviceEvent* event);

//TODO : add default button mapping and button repeat for gamepad and other InputDevice

class Controller : InputDevice {
public:
    Controller(SDL_JoystickID id);
    ~Controller();
    bool is(SDL_JoystickID device);
    bool is_connected();
    void open(SDL_JoystickID device);
    void close();
    void process_event(SDL_Event* event);
    void set_axis_callback(Uint8 type, controller_axis_callback callback);
    void set_button_callback(Uint8 type, controller_button_callback callback);
    void set_connection_callback(controller_callback callback);
    void set_disconnect_callback(controller_callback callback);
    void rumble(float strength, int duration);
    SDL_JoystickID id();
    float axis(SDL_GameControllerAxis);
    Uint8 button(SDL_GameControllerButton);

private:
    SDL_GameController* _gamepad{ nullptr };
    SDL_Haptic* _haptic{ nullptr };
    SDL_JoystickID _instance_id{ -1 };
    bool _is_connected{ false };
    controller_callback _connection_callback{ nullptr };
    controller_callback _disconnect_callback{ nullptr };
    std::map<Uint8, controller_axis_callback> _axis_callbacks;
    std::map<Uint8, controller_button_callback> _button_callbacks;
    Controller();
};

class GameController : InputDevice {
public:
    static Controller* get(int index);
    static void remove(int index);
    static int get_controller_index(SDL_JoystickID device);
    virtual void process_event(SDL_Event* event);

private:
    std::map<SDL_JoystickID, Controller*> _controllers;
    static GameController* _get();
    static GameController* _instance;
    GameController();
};