/*
* @Author: gpinchon
* @Date:   2021-05-21 11:56:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 18:23:51
*/

#include <Driver/SDL2/GameController.hpp>
#include <Event/EventsManager.hpp>
#include <Event/Event.hpp>
#include <Event/GameController.hpp>
#include <Event/InputDevice/GameController.hpp>
#include <Event/Signal.hpp>

#include <array>
#include <iostream>
#include <stdexcept>

#include <glm/vec3.hpp>

#include <SDL.h>
#include <SDL_gamecontroller.h>

namespace SDL2 {
namespace GameController {
    const static std::map<::GameController::Button, SDL_GameControllerButton> s_buttonLUT {
        { ::GameController::Button::Unknown, SDL_CONTROLLER_BUTTON_INVALID },
        { ::GameController::Button::A, SDL_CONTROLLER_BUTTON_A },
        { ::GameController::Button::B, SDL_CONTROLLER_BUTTON_B },
        { ::GameController::Button::X, SDL_CONTROLLER_BUTTON_X },
        { ::GameController::Button::Y, SDL_CONTROLLER_BUTTON_Y },
        { ::GameController::Button::Back, SDL_CONTROLLER_BUTTON_BACK },
        { ::GameController::Button::Guide, SDL_CONTROLLER_BUTTON_GUIDE },
        { ::GameController::Button::Start, SDL_CONTROLLER_BUTTON_START },
        { ::GameController::Button::LeftStick, SDL_CONTROLLER_BUTTON_LEFTSTICK },
        { ::GameController::Button::RightStick, SDL_CONTROLLER_BUTTON_RIGHTSTICK },
        { ::GameController::Button::Leftshoulder, SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
        { ::GameController::Button::Rightshoulder, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
        { ::GameController::Button::DpadUp, SDL_CONTROLLER_BUTTON_DPAD_UP },
        { ::GameController::Button::DpadDown, SDL_CONTROLLER_BUTTON_DPAD_DOWN },
        { ::GameController::Button::DpadLeft, SDL_CONTROLLER_BUTTON_DPAD_LEFT },
        { ::GameController::Button::DpadRight, SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
        { ::GameController::Button::Misc1, SDL_CONTROLLER_BUTTON_MISC1 },
        { ::GameController::Button::Paddle1, SDL_CONTROLLER_BUTTON_PADDLE1 },
        { ::GameController::Button::Paddle2, SDL_CONTROLLER_BUTTON_PADDLE2 },
        { ::GameController::Button::Paddle3, SDL_CONTROLLER_BUTTON_PADDLE3 },
        { ::GameController::Button::Paddle4, SDL_CONTROLLER_BUTTON_PADDLE4 },
        { ::GameController::Button::Touchpad, SDL_CONTROLLER_BUTTON_TOUCHPAD },
        { ::GameController::Button::MaxValue, SDL_CONTROLLER_BUTTON_MAX }
    };
    //Lambda no jutsu
    const static auto s_reverseButtonLUT = [&]() {
        std::map<SDL_GameControllerButton, ::GameController::Button> map;
        for (const auto& [key, value] : s_buttonLUT)
            map[value] = key;
        return map;
    }();
    const static std::map<::GameController::Axis, SDL_GameControllerAxis> s_axisLUT {
        { ::GameController::Axis::Unknown, SDL_CONTROLLER_AXIS_INVALID },
        { ::GameController::Axis::LeftX, SDL_CONTROLLER_AXIS_LEFTX },
        { ::GameController::Axis::LeftY, SDL_CONTROLLER_AXIS_LEFTY },
        { ::GameController::Axis::RightX, SDL_CONTROLLER_AXIS_RIGHTX },
        { ::GameController::Axis::RightY, SDL_CONTROLLER_AXIS_RIGHTY },
        { ::GameController::Axis::TriggerLeft, SDL_CONTROLLER_AXIS_TRIGGERLEFT },
        { ::GameController::Axis::TriggerRight, SDL_CONTROLLER_AXIS_TRIGGERRIGHT },
        { ::GameController::Axis::MaxValue, SDL_CONTROLLER_AXIS_MAX }
    };
    const static auto s_reverseAxisLUT = [&]() {
        std::map<SDL_GameControllerAxis, ::GameController::Axis> map;
        for (const auto& [key, value] : s_axisLUT)
            map[value] = key;
        return map;
    }();
    auto GetAxis(::GameController::Axis axis)
    {
        return s_axisLUT.at(axis);
    }
    auto GetButton(::GameController::Button button)
    {
        return s_buttonLUT.at(button);
    }
    auto GetAxis(const SDL_GameControllerAxis& axis)
    {
        return s_reverseAxisLUT.at(axis);
    }
    auto GetButton(const SDL_GameControllerButton& button)
    {
        return s_reverseButtonLUT.at(button);
    }
    static inline auto GetPlayerIndex(SDL_JoystickID id) {
        auto gameController = SDL_GameControllerFromInstanceID(id);
        return SDL_GameControllerGetPlayerIndex(gameController);
    }
    static inline auto GetGameController(uint8_t playerIndex) {
        return SDL_GameControllerFromPlayerIndex(playerIndex);
    }
    Event::GameControllerAxis CreateEventData(const SDL_ControllerAxisEvent& event)
    {
        return {
            GetPlayerIndex(event.which),
            GetAxis(SDL_GameControllerAxis(event.axis)),
            event.value / float(std::numeric_limits<int16_t>::max())
        };
    }
    Event::GameControllerButton CreateEventData(const SDL_ControllerButtonEvent& event)
    {
        return {
            GetPlayerIndex(event.which),
            GetButton(SDL_GameControllerButton(event.button)),
            event.state == SDL_PRESSED
        };
    }
    Event::GameControllerDevice CreateEventData(const SDL_ControllerDeviceEvent& event)
    {
        return {
            event.type == SDL_CONTROLLERDEVICEADDED ? event.which : GetPlayerIndex(event.which)
        };
    }
};
};

namespace GameController {

template<class T, class Compare>
constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
{
    assert(!comp(hi, lo));
    return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
    return clamp(v, lo, hi, std::less{});
}

InputDevice::InputDevice()
{
    if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER)) {
        if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0)
            throw std::runtime_error(SDL_GetError());
        SDL_GameControllerEventState(SDL_ENABLE);
    }
    EventsManager::On(Event::Type::ControllerAxisMotion).ConnectMember(this, &InputDevice::_ProcessEvent);
    EventsManager::On(Event::Type::ControllerButtonDown).ConnectMember(this, &InputDevice::_ProcessEvent);
    EventsManager::On(Event::Type::ControllerButtonUp).ConnectMember(this, &InputDevice::_ProcessEvent);
    EventsManager::On(Event::Type::ControllerDeviceAdded).ConnectMember(this, &InputDevice::_ProcessEvent);
    EventsManager::On(Event::Type::ControllerDeviceRemoved).ConnectMember(this, &InputDevice::_ProcessEvent);
}

InputDevice::~InputDevice()
{
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void InputDevice::OpenController(int32_t index)
{
    if (!SDL_IsGameController(index))
        return;
    SDL_GameControllerSetPlayerIndex(SDL_GameControllerOpen(index), index);
    std::cout << "Game Controller Connected : " << index << std::endl;
}

void InputDevice::CloseController(int32_t index)
{
    SDL_GameControllerClose(SDL_GameControllerFromPlayerIndex(index));
    std::cout << "Game Controller Closed : " << index << std::endl;
}

Signal<const Event::GameControllerAxis&>& InputDevice::OnAxis(uint8_t index, const GameController::Axis& axis)
{
    return _onAxis[index].at(size_t(axis));
}

Signal<const Event::GameControllerButton&>& InputDevice::OnButton(uint8_t index, const GameController::Button& button)
{
    return _onButton[index].at(size_t(button));
}

Signal<const Event::GameControllerButton&>& InputDevice::OnButtonUp(uint8_t index, const GameController::Button& button)
{
    return _onButtonUp[index].at(size_t(button));
}

Signal<const Event::GameControllerButton&>& InputDevice::OnButtonDown(uint8_t index, const GameController::Button& button)
{
    return _onButtonDown[index].at(size_t(button));
}

Signal<const Event::GameControllerDevice&>& InputDevice::OnConnection(uint8_t index)
{
    return _onConnection[index];
}

Signal<const Event::GameControllerDevice&>& InputDevice::OnDisconnection(uint8_t index)
{
    return _onDisconnection[index];
}

bool InputDevice::Rumble(uint8_t index, float lowFrequency, float highFrequency, uint32_t duration)
{
    lowFrequency = clamp(lowFrequency, 0.f, 1.f);
    highFrequency = clamp(highFrequency, 0.f, 1.f);
    constexpr auto norm = std::numeric_limits<uint16_t>::max();
    return SDL_GameControllerRumble(SDL2::GameController::GetGameController(index), lowFrequency * norm, highFrequency * norm, duration) == 0;
}

bool InputDevice::RumbleTriggers(uint8_t index, float left_rumble, float right_rumble, uint32_t duration)
{
    left_rumble = clamp(left_rumble, 0.f, 1.f);
    right_rumble = clamp(right_rumble, 0.f, 1.f);
    constexpr auto norm = std::numeric_limits<uint16_t>::max();
    return SDL_GameControllerRumbleTriggers(SDL2::GameController::GetGameController(index), left_rumble * norm, right_rumble * norm, duration) == 0;
}

bool InputDevice::SetLED(uint8_t index, const glm::vec3& color)
{
    auto r = clamp(color.r, 0.f, 1.f) * 255;
    auto g = clamp(color.g, 0.f, 1.f) * 255;
    auto b = clamp(color.b, 0.f, 1.f) * 255;
    return SDL_GameControllerSetLED(SDL2::GameController::GetGameController(index), r, g, b) == 0;
}

float InputDevice::GetAxis(uint8_t index, GameController::Axis axis)
{
    auto value = SDL_GameControllerGetAxis(SDL2::GameController::GetGameController(index), SDL2::GameController::GetAxis(axis)) / 32767.f;
    if (value < 0 && value > -0.1) {
        value = 0;
    }
    else if (value > 0 && value < 0.1) {
        value = 0;
    }
    return (value);
}

bool InputDevice::GetButton(uint8_t index, GameController::Button button)
{
    return SDL_GameControllerGetButton(SDL2::GameController::GetGameController(index), SDL2::GameController::GetButton(button));
}

void InputDevice::_ProcessEvent(const Event& event)
{
    switch (event.type) {
    case Event::Type::ControllerAxisMotion: {
        auto& axisEvent{ event.Get<Event::GameControllerAxis>() };
        auto index{ axisEvent.id };
        auto signal = _onAxis.find(index);
        if (signal != _onAxis.end())
            signal->second.at(size_t(axisEvent.axis))(axisEvent);
        break;
    }
    case Event::Type::ControllerButtonDown: {
        auto& buttonEvent{ event.Get<Event::GameControllerButton>() };
        auto index{ buttonEvent.id };
        auto signal = _onButton.find(index);
        if (signal != _onButton.end())
            signal->second.at(size_t(buttonEvent.button))(buttonEvent);
        signal = _onButtonDown.find(index);
        if (signal != _onButtonDown.end())
            signal->second.at(size_t(buttonEvent.button))(buttonEvent);
        break;
    }
    case Event::Type::ControllerButtonUp: {
        auto& buttonEvent{ event.Get<Event::GameControllerButton>() };
        auto index{ buttonEvent.id };
        auto signal = _onButton.find(index);
        if (signal != _onButton.end())
            signal->second.at(size_t(buttonEvent.button))(buttonEvent);
        signal = _onButtonUp.find(index);
        if (signal != _onButtonUp.end())
            signal->second.at(size_t(buttonEvent.button))(buttonEvent);
        break;
    }
    case Event::Type::ControllerDeviceAdded: {
        auto& deviceAddedEvent{ event.Get<Event::GameControllerDevice>() };
        auto index{ deviceAddedEvent.id };
        OpenController(index);
        auto signal = _onConnection.find(index);
        if (signal != _onConnection.end())
            signal->second(deviceAddedEvent);
        break;
    }
    case Event::Type::ControllerDeviceRemoved: {
        auto& deviceRemovedEvent{ event.Get<Event::GameControllerDevice>() };
        auto index{ deviceRemovedEvent.id };
        CloseController(index);
        auto signal = _onDisconnection.find(index);
        if (signal != _onDisconnection.end())
            signal->second(deviceRemovedEvent);
        break;
    }
    case Event::Type::ControllerDeviceRemapped:
        break;
    case Event::Type::ControllerTouchpadDown:
        break;
    case Event::Type::ControllerTouchpadMotion:
        break;
    case Event::Type::ControllerTouchpadUp:
        break;
    case Event::Type::ControllerSensorUpdate:
        break;
    default:
        throw std::runtime_error("Incorrect Event::Type");
    }
}
};