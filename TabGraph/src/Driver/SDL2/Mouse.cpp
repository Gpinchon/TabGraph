/*
* @Author: gpinchon
* @Date:   2021-05-21 22:03:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 18:23:55
*/

#include <Driver/SDL2/Mouse.hpp>
#include <Event/InputDevice/Mouse.hpp>
#include <Event/EventsManager.hpp>
#include <Window.hpp>

#include <SDL_events.h>
#include <SDL_mouse.h> // for SDL_GetMouseState, SDL_GetRelativeMouseMode

namespace SDL2 {
namespace Mouse {
    const static std::unordered_map<uint8_t, ::Mouse::Button> s_buttonLUT {
        { SDL_BUTTON_LEFT, ::Mouse::Button::Left },
        { SDL_BUTTON_MIDDLE, ::Mouse::Button::Middle },
        { SDL_BUTTON_RIGHT, ::Mouse::Button::Right },
        { SDL_BUTTON_X1, ::Mouse::Button::X1 },
        { SDL_BUTTON_X2, ::Mouse::Button::X2 }
    };
    const static auto s_reversebuttonLUT { [&]() {
        std::unordered_map<::Mouse::Button, uint8_t> map;
        for (const auto& [key, value] : s_buttonLUT)
            map[value] = key;
        return map;
    }() };
    Event::MouseButton CreateEventData(const SDL_MouseButtonEvent& event)
    {
        return {
            Window::Get(event.windowID),
            ::Mouse::GetPosition(),
            s_buttonLUT.at(event.button)
        };
    }
    Event::MouseWheel CreateEventData(const SDL_MouseWheelEvent& event)
    {
        return {
            Window::Get(event.windowID),
            ::Mouse::GetPosition(),
            event.direction == SDL_MOUSEWHEEL_FLIPPED ? ::Mouse::WheelDirection::Flipped : ::Mouse::WheelDirection::Natural,
            glm::ivec2(event.x, event.y)
        };
    }
    Event::MouseMove CreateEventData(const SDL_MouseMotionEvent& event)
    {
        return {
            Window::Get(event.windowID),
            ::Mouse::GetPosition(),
            glm::ivec2(event.xrel, event.yrel)
        };
    }
}
}

namespace Mouse {
InputDevice::InputDevice() {
    EventsManager::On(Event::Type::MouseMotion).ConnectMember(this, &InputDevice::_ProcessEvent);
    EventsManager::On(Event::Type::MouseButtonDown).ConnectMember(this, &InputDevice::_ProcessEvent);
    EventsManager::On(Event::Type::MouseButtonUp).ConnectMember(this, &InputDevice::_ProcessEvent);
    EventsManager::On(Event::Type::MouseWheel).ConnectMember(this, &InputDevice::_ProcessEvent);
}
bool InputDevice::GetRelative() const
{
    return SDL_GetRelativeMouseMode();
}
void InputDevice::SetRelative(bool value)
{
    SDL_SetRelativeMouseMode(SDL_bool(value));
}
bool InputDevice::GetButtonState(Button button) const
{
    auto mask = SDL_GetMouseState(nullptr, nullptr);
    return (mask & SDL_BUTTON(SDL2::Mouse::s_reversebuttonLUT.at(button)));
}
glm::ivec2 InputDevice::GetPosition() const
{
    glm::ivec2 pos { 0 };
    SDL_GetMouseState(&pos.x, &pos.y);
    return pos;
}
};