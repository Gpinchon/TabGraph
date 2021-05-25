/*
* @Author: gpinchon
* @Date:   2021-05-20 15:18:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 16:07:45
*/

#pragma once

#include <Event/Event.hpp>
#include <Event/Keyboard.hpp>
#include <Event/InputDevice/InputDevice.hpp>
#include <Event/Signal.hpp>

#include <array>

namespace Keyboard {
struct InputDevice : ::InputDevice {
    InputDevice();
    bool GetKeyState(Keyboard::Key);
    virtual void ProcessEvent(const Event& event) override
    {
        auto& keyboardEvent{ event.Get<Event::Keyboard>() };
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
    std::array<Signal<const Event::Keyboard&>, size_t(Keyboard::Key::MaxValue)> onKey;
    std::array<Signal<const Event::Keyboard&>, size_t(Keyboard::Key::MaxValue)> onKeyUp;
    std::array<Signal<const Event::Keyboard&>, size_t(Keyboard::Key::MaxValue)> onKeyDown;
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
