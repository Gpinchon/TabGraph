/*
* @Author: gpinchon
* @Date:   2021-05-20 15:18:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:53
*/

#pragma once

#include <Events/Event.hpp>
#include <Events/Keyboard.hpp>
#include <Events/Signal.hpp>

#include <array>

namespace Keyboard {
struct InputDevice : Trackable {
    InputDevice();
    bool GetKeyState(Keyboard::Key);
    std::array<Signal<const Event::Keyboard&>, size_t(Keyboard::Key::MaxValue)> onKey;
    std::array<Signal<const Event::Keyboard&>, size_t(Keyboard::Key::MaxValue)> onKeyUp;
    std::array<Signal<const Event::Keyboard&>, size_t(Keyboard::Key::MaxValue)> onKeyDown;

private:
    void _ProcessEvent(const Event& event)
    {
        auto& keyboardEvent { event.Get<Event::Keyboard>() };
        switch (event.type) {
        case Event::Type::KeyDown: {
            onKey.at(size_t(keyboardEvent.key))(keyboardEvent);
            onKeyDown.at(size_t(keyboardEvent.key))(keyboardEvent);
            break;
        }
        case Event::Type::KeyUp: {
            onKey.at(size_t(keyboardEvent.key))(keyboardEvent);
            onKeyUp.at(size_t(keyboardEvent.key))(keyboardEvent);
            break;
        }
        }
    }
};
};

struct SDL_KeyboardEvent;
struct SDL_TextEditingEvent;
struct SDL_TextInputEvent;

namespace SDL2 {
namespace Keyboard {
    Event::Keyboard CreateEventData(const SDL_KeyboardEvent& event);
    Event::TextEdit CreateEventData(const SDL_TextEditingEvent& event);
    Event::TextInput CreateEventData(const SDL_TextInputEvent& event);
};
};
