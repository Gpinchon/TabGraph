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

/*void Keyboard::set_callback(SDL_Scancode key, keyboard_callback callback)
{
    _get()._callbacks[key] = callback;
}*/

void Keyboard::AddKeyCallback(SDL_Scancode key, std::shared_ptr<Callback<void(const SDL_KeyboardEvent&)>> callback)
{
    _get()._callbacks[key].push_back(callback);
}

void Keyboard::RemoveKeyCallback(SDL_Scancode key, std::shared_ptr<Callback<void(const SDL_KeyboardEvent&)>> callback)
{
    _get()._callbacks[key].erase(std::remove(_get()._callbacks[key].begin(), _get()._callbacks[key].end(), callback), _get()._callbacks[key].end());
}

void Keyboard::process_event(SDL_Event* event)
{
    auto callback = _get()._callbacks[event->key.keysym.scancode];
    for (const auto& callback : callback)
        callback->Call(event->key);
    /*if (callback != _get()._callbacks.end())
        callback->second(&event->key);*/
}
