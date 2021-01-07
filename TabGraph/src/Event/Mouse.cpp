/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 20:05:47
*/

#include "Event/Mouse.hpp"
#include "Event/Events.hpp" // for Events
#include <SDL_mouse.h> // for SDL_GetMouseState, SDL_GetRelativeMouseMode

Mouse* Mouse::_instance = nullptr;

Mouse::Mouse()
{
    Events::Add(this, SDL_MOUSEMOTION);
    Events::Add(this, SDL_MOUSEBUTTONDOWN);
    Events::Add(this, SDL_MOUSEBUTTONUP);
    Events::Add(this, SDL_MOUSEWHEEL);
}

void Mouse::process_event(SDL_Event* event)
{
    switch (event->type) {
    case SDL_MOUSEMOTION: {
        _get()->_onMove(event->motion);
        break;
    }
    case SDL_MOUSEBUTTONDOWN: {
        _get()->_onButtonDown.at(event->button.button)(event->button);
        _get()->_onButton.at(event->button.button)(event->button);
        break;
    }

    case SDL_MOUSEBUTTONUP: {
        _get()->_onButtonUp.at(event->button.button)(event->button);
        _get()->_onButton.at(event->button.button)(event->button);
        break;
    }
    case SDL_MOUSEWHEEL: {
        _get()->_onWheel(event->wheel);
        break;
    }
    }
}

bool Mouse::relative()
{
    return SDL_GetRelativeMouseMode();
}

void Mouse::set_relative(bool value)
{
    SDL_SetRelativeMouseMode(SDL_bool(value));
}

bool Mouse::button(Uint8 button)
{
    auto mask = SDL_GetMouseState(nullptr, nullptr);
    return (mask & SDL_BUTTON(button));
}

void Mouse::position(Sint32& x, Sint32& y)
{
    SDL_GetMouseState(&x, &y);
}

Signal<const SDL_MouseMotionEvent&>& Mouse::OnMove()
{
    return _get()->_onMove;
}

Signal<const SDL_MouseWheelEvent&>& Mouse::OnWheel()
{
    return _get()->_onWheel;
}

Signal<const SDL_MouseButtonEvent&>& Mouse::OnButton(uint8_t button)
{
    return _get()->_onButton.at(button);
}

Signal<const SDL_MouseButtonEvent&>& Mouse::OnButtonDown(uint8_t button)
{
    return _get()->_onButtonDown.at(button);
}

Signal<const SDL_MouseButtonEvent&>& Mouse::OnButtonUp(uint8_t button)
{
    return _get()->_onButtonUp.at(button);
}

Mouse* Mouse::_get()
{
    if (_instance == nullptr)
        _instance = new Mouse();
    return (_instance);
}