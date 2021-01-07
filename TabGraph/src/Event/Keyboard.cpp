/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-16 15:37:23
*/

#include "Event/Keyboard.hpp"
#include "Event/Events.hpp" // for Events
#include <SDL_keyboard.h> // for SDL_GetKeyboardState, SDL_Keysym
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

bool Keyboard::key(SDL_Scancode key)
{
    return (SDL_GetKeyboardState(nullptr)[key]);
}

Signal<const SDL_KeyboardEvent&>& Keyboard::OnKey(SDL_Scancode key)
{
    return _get()._onKey.at(key);
}

Signal<const SDL_KeyboardEvent&>& Keyboard::OnKeyUp(SDL_Scancode key)
{
    return _get()._onKeyUp.at(key);
}

Signal<const SDL_KeyboardEvent&>& Keyboard::OnKeyDown(SDL_Scancode key)
{
    return _get()._onKeyDown.at(key);
}

void Keyboard::process_event(SDL_Event* event)
{
    switch (event->type) {
    case SDL_KEYDOWN: {
        _get()._onKey.at(event->key.keysym.scancode)(event->key);
        _get()._onKeyDown.at(event->key.keysym.scancode)(event->key);
        break;
    }
    case SDL_KEYUP: {
        _get()._onKey.at(event->key.keysym.scancode)(event->key);
        _get()._onKeyUp.at(event->key.keysym.scancode)(event->key);
        break;
    }
    }
}
