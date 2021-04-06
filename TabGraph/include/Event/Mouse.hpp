/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:27
*/

#pragma once

#include "Event/InputDevice.hpp" // for InputDevice
#include "Event/Signal.hpp"

#include <SDL_events.h> // for SDL_Event, SDL_MouseButtonEvent, SDL_Mo...
#include <SDL_stdinc.h> // for Sint32, Uint8, SDL_bool
#include <array> // for array

class Mouse : InputDevice {
public:
    void process_event(SDL_Event*);
    static bool relative();
    static void set_relative(bool relative);
    static bool button(Uint8 button);
    static void position(Sint32& x, Sint32& y);
    static Signal<const SDL_MouseMotionEvent&>& OnMove();
    static Signal<const SDL_MouseWheelEvent&>& OnWheel();
    static Signal<const SDL_MouseButtonEvent&>& OnButton(uint8_t button);
    static Signal<const SDL_MouseButtonEvent&>& OnButtonDown(uint8_t button);
    static Signal<const SDL_MouseButtonEvent&>& OnButtonUp(uint8_t button);

private:
    Mouse();
    static Mouse* _get();
    static Mouse* _instance;
    Signal<const SDL_MouseMotionEvent&> _onMove {};
    Signal<const SDL_MouseWheelEvent&> _onWheel {};
    std::array<Signal<const SDL_MouseButtonEvent&>, 16> _onButtonDown { {} };
    std::array<Signal<const SDL_MouseButtonEvent&>, 16> _onButtonUp { {} };
    std::array<Signal<const SDL_MouseButtonEvent&>, 16> _onButton { {} };
};