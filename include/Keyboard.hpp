/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 17:56:51
*/

#pragma once

#include <SDL2/SDL_events.h>    // for SDL_Event, SDL_KeyboardEvent
#include <SDL2/SDL_scancode.h>  // for SDL_Scancode
#include <SDL2/SDL_stdinc.h>    // for Uint8
#include <map>                  // for map
#include "InputDevice.hpp"      // for InputDevice

typedef void (*keyboard_callback)(SDL_KeyboardEvent* event);

class Keyboard : InputDevice {
public:
    static Uint8 key(SDL_Scancode);
    static void set_callback(SDL_Scancode key, keyboard_callback callback);
    void process_event(SDL_Event*) override;

private:
    static Keyboard& _get();
    static Keyboard* _instance;
    std::map<SDL_Scancode, keyboard_callback> _callbacks;
    Keyboard();
};