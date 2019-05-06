/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:27:58
*/

#pragma once

#include "Events.hpp"
#include "InputDevice.hpp"

typedef void (*keyboard_callback)(SDL_KeyboardEvent* event);

class Keyboard : InputDevice {
public:
    static Uint8 key(SDL_Scancode);
    static void set_callback(SDL_Scancode key, keyboard_callback callback);
    void process_event(SDL_Event*);

private:
    static Keyboard& _get();
    static Keyboard* _instance;
    std::map<SDL_Scancode, keyboard_callback> _callbacks;
    Keyboard();
};