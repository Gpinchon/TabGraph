/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:05:03
*/

#include "Input/Keyboard.hpp"
#include "Input/Events.hpp" // for Events
#include <SDL2/SDL_keyboard.h> // for SDL_GetKeyboardState, SDL_Keysym
#include <utility> // for pair

Keyboard* Keyboard::_instance = new Keyboard();

Keyboard::Keyboard()
{
    Events::Add(this, SDL_KEYUP);
    Events::Add(this, SDL_KEYDOWN);
}

Keyboard& Keyboard::_get()
{
    return (*_instance);
}

Uint8 Keyboard::key(SDL_Scancode key)
{
    return (SDL_GetKeyboardState(nullptr)[key]);
}

void Keyboard::set_callback(SDL_Scancode key, keyboard_callback callback)
{
    _get()._callbacks[key] = callback;
}

void Keyboard::process_event(SDL_Event* event)
{
    auto callback = _get()._callbacks.find(event->key.keysym.scancode);
    if (callback != _get()._callbacks.end())
        callback->second(&event->key);
}
