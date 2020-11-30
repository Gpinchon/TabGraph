/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 10:30:37
*/

#pragma once

#include "Input/InputDevice.hpp" // for InputDevice
#include <SDL_events.h> // for SDL_Event, SDL_MouseButtonEvent, SDL_Mo...
#include <SDL_stdinc.h> // for Sint32, Uint8, SDL_bool
#include <array> // for array

typedef void (*mouse_motion_callback)(SDL_MouseMotionEvent* event);
typedef void (*mouse_button_callback)(SDL_MouseButtonEvent* event);
typedef void (*mouse_wheel_callback)(SDL_MouseWheelEvent* event);

class Mouse : InputDevice {
public:
    void process_event(SDL_Event*);
    static bool relative();
    static void set_relative(bool relative);
    static bool button(Uint8 button);
    static void position(Sint32& x, Sint32& y);
    static void set_move_callback(mouse_motion_callback);
    static void set_wheel_callback(mouse_wheel_callback);
    static void set_button_callback(mouse_button_callback, Uint8 button);

private:
    Mouse();
    static Mouse* _get();
    static Mouse* _instance;
    std::array<mouse_button_callback, 5> _button_callbacks { { nullptr } };
    mouse_motion_callback _move_callback { nullptr };
    mouse_wheel_callback _wheel_callback { nullptr };
};