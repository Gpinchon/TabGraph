/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 17:57:07
*/

#include "Keyboard.hpp"
#include "Events.hpp" // for Events
#include <SDL2/SDL_keyboard.h> // for SDL_GetKeyboardState, SDL_Keysym
#include <utility> // for pair

Keyboard* Keyboard::_instance = new Keyboard();

Keyboard::Keyboard()
{
    Events::add(this, SDL_KEYUP);
    Events::add(this, SDL_KEYDOWN);
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
