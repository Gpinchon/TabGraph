/*
* @Author: gpinchon
* @Date:   2021-05-21 22:04:13
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 13:46:23
*/
#pragma once

#include <Event/Event.hpp>
#include <Event/InputDevice/InputDevice.hpp>
#include <Event/Mouse.hpp>
#include <Event/Signal.hpp>

#include <array>
#include <stdexcept>

namespace Mouse {
struct InputDevice : ::InputDevice {
    InputDevice();
    virtual void ProcessEvent(const Event& event) override
    {
        switch (event.type) {
        case Event::Type::MouseMotion: {
            onMove(event.Get<Event::MouseMove>());
            break;
        }
        case Event::Type::MouseButtonDown: {
            const auto& eventData { event.Get<Event::MouseButton>() };
            onButtonDown.at(size_t(eventData.button))(eventData);
            onButton.at(size_t(eventData.button))(eventData);
            break;
        }

        case Event::Type::MouseButtonUp: {
            const auto& eventData { event.Get<Event::MouseButton>() };
            onButtonUp.at(size_t(eventData.button))(eventData);
            onButton.at(size_t(eventData.button))(eventData);
            break;
        }
        case Event::Type::MouseWheel: {
            onWheel(event.Get<Event::MouseWheel>());
            break;
        }
        default:
            throw std::runtime_error("Incorrect Event Type");
        }
    }
    ///@return true if mouse is in relative motion mode
    bool GetRelative() const;
    void SetRelative(bool relative);
    ///@return true if button us pressed
    bool GetButtonState(Button) const;
    ///@return the current mouse position in pixels
    glm::ivec2 GetPosition() const;
    Signal<const Event::MouseMove&> onMove {};
    Signal<const Event::MouseWheel&> onWheel {};
    std::array<Signal<const Event::MouseButton&>, size_t(Mouse::Button::MaxValue)> onButtonDown { {} };
    std::array<Signal<const Event::MouseButton&>, size_t(Mouse::Button::MaxValue)> onButtonUp { {} };
    std::array<Signal<const Event::MouseButton&>, size_t(Mouse::Button::MaxValue)> onButton { {} };
};
};

struct SDL_MouseButtonEvent;
struct SDL_MouseWheelEvent;
struct SDL_MouseMotionEvent;

namespace SDL2 {
namespace Mouse {
    Event::MouseButton CreateEventData(const SDL_MouseButtonEvent& event);
    Event::MouseWheel CreateEventData(const SDL_MouseWheelEvent& event);
    Event::MouseMove CreateEventData(const SDL_MouseMotionEvent& event);
}
}