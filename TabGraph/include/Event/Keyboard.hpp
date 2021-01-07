/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 17:56:51
*/

#pragma once

#include "Signal.hpp"
#include "InputDevice.hpp" // for InputDevice

#include <SDL_events.h> // for SDL_Event, SDL_KeyboardEvent
#include <SDL_scancode.h> // for SDL_Scancode
#include <array> // for map


class Keyboard : InputDevice {
public:
    static bool key(SDL_Scancode);
    static Signal<const SDL_KeyboardEvent&>& OnKey(SDL_Scancode key);
    static Signal<const SDL_KeyboardEvent&>& OnKeyUp(SDL_Scancode key);
    static Signal<const SDL_KeyboardEvent&>& OnKeyDown(SDL_Scancode key);
    void process_event(SDL_Event*) override;

private:
    static Keyboard& _get();
    static Keyboard* _instance;
    std::array<Signal<const SDL_KeyboardEvent&>, SDL_NUM_SCANCODES> _onKey;
    std::array<Signal<const SDL_KeyboardEvent&>, SDL_NUM_SCANCODES> _onKeyUp;
    std::array<Signal<const SDL_KeyboardEvent&>, SDL_NUM_SCANCODES> _onKeyDown;
    Keyboard();
};