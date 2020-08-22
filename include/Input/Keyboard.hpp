/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 17:56:51
*/

#pragma once

#include "Callback.hpp"
#include "InputDevice.hpp" // for InputDevice

#include <SDL_events.h> // for SDL_Event, SDL_KeyboardEvent
#include <SDL_scancode.h> // for SDL_Scancode
#include <map> // for map

typedef void (*keyboard_callback)(SDL_KeyboardEvent* event);

class Keyboard : InputDevice {
public:
    static bool key(SDL_Scancode);
    //static void set_callback(SDL_Scancode key, keyboard_callback callback);
    static void AddKeyCallback(SDL_Scancode key, std::shared_ptr<Callback<void(const SDL_KeyboardEvent&)>> callback);
    static void RemoveKeyCallback(SDL_Scancode key, std::shared_ptr<Callback<void(const SDL_KeyboardEvent&)>> callback);
    void process_event(SDL_Event*) override;

private:
    static Keyboard& _get();
    static Keyboard* _instance;
    //std::map<SDL_Scancode, keyboard_callback> _callbacks;
    std::map<SDL_Scancode, std::vector<std::shared_ptr<Callback<void(const SDL_KeyboardEvent&)>>>> _callbacks;
    Keyboard();
};