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

#include <SDL.h>
#include <SDL_gamecontroller.h>
#include <SDL_haptic.h>

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
    Event::GameControllerAxis CreateEventData(const SDL_ControllerAxisEvent& event)
    {
        return {
            event.which,
            GetAxis(SDL_GameControllerAxis(event.axis)),
            event.value / float(std::numeric_limits<int16_t>::max())
        };
    }
    Event::GameControllerButton CreateEventData(const SDL_ControllerButtonEvent& event)
    {
        return {
            event.which,
            GetButton(SDL_GameControllerButton(event.button)),
            event.state == SDL_PRESSED
        };
    }
    Event::GameControllerDevice CreateEventData(const SDL_ControllerDeviceEvent& event)
    {
        return {
            event.which
        };
    }
};
};

namespace GameController {
struct Gamepad {
    Gamepad() = delete;
    Gamepad(int32_t id);
    ~Gamepad();
    bool Is(int32_t device);
    bool IsConnected();
    void Open(int32_t device);
    void Close();
    void Rumble(float strength, int duration);
    int32_t GetId();
    float GetAxis(GameController::Axis);
    bool GetButton(GameController::Button);
    SDL_GameController* _gamepad { nullptr };
    SDL_Haptic* _haptic { nullptr };
    int32_t _instanceId { -1 };
    bool _is_connected { false };
    Signal<const Event::GameControllerDevice&> onConnection;
    Signal<const Event::GameControllerDevice&> onDisconnection;
    std::array<Signal<const Event::GameControllerAxis&>, size_t(GameController::Axis::MaxValue)> onAxis;
    std::array<Signal<const Event::GameControllerButton&>, size_t(GameController::Button::MaxValue)> onButton;
    std::array<Signal<const Event::GameControllerButton&>, size_t(GameController::Button::MaxValue)> onButtonUp;
    std::array<Signal<const Event::GameControllerButton&>, size_t(GameController::Button::MaxValue)> onButtonDown;
};

Gamepad::Gamepad(int32_t device)
{
    Open(device);
}

Gamepad::~Gamepad()
{
    Close();
}

bool Gamepad::Is(int32_t device)
{
    return GetId() == device;
}

bool Gamepad::IsConnected()
{
    return _is_connected;
}

void Gamepad::Open(int32_t device)
{
    if (_is_connected || !SDL_IsGameController(device)) {
        return;
    }
    _gamepad = SDL_GameControllerOpen(device);
    auto j = SDL_GameControllerGetJoystick(_gamepad);
    _instanceId = SDL_JoystickInstanceID(j);
    _is_connected = true;
    if (SDL_JoystickIsHaptic(j)) {
        _haptic = SDL_HapticOpenFromJoystick(j);
        if (SDL_HapticRumbleSupported(_haptic)) {
            SDL_HapticRumbleInit(_haptic);
        }
    }
    std::cout << "Joystick ID " << _instanceId << std::endl;
}

void Gamepad::Close()
{
    if (!_is_connected)
        return;
    if (_haptic) {
        SDL_HapticClose(_haptic);
        _haptic = nullptr;
    }
    SDL_GameControllerClose(_gamepad);
    _gamepad = nullptr;
    _instanceId = -1;
    _is_connected = false;
}

void Gamepad::Rumble(float strength, int duration)
{
    SDL_HapticRumblePlay(_haptic, strength, duration);
}

float Gamepad::GetAxis(GameController::Axis axis)
{
    auto value = SDL_GameControllerGetAxis(_gamepad, SDL2::GameController::GetAxis(axis)) / 32767.f;
    if (value < 0 && value > -0.1) {
        value = 0;
    } else if (value > 0 && value < 0.1) {
        value = 0;
    }
    return (value);
}

bool Gamepad::GetButton(GameController::Button button)
{
    return SDL_GameControllerGetButton(_gamepad, SDL2::GameController::GetButton(button));
}

SDL_JoystickID Gamepad::GetId()
{
    return _instanceId;
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

    //if (!SDL_WasInit(SDL_INIT_JOYSTICK)) {
    //    if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
    //      throw std::runtime_error(SDL_GetError());
    //    SDL_JoystickEventState(SDL_ENABLE);
    //}
    //EventsManager::Add(this, Event::Type::JoyAxisMotion);
    //EventsManager::Add(this, Event::Type::JoyButtonDown);
    //EventsManager::Add(this, Event::Type::JoyButtonUp);
    //EventsManager::Add(this, Event::Type::JoyDeviceAdded);
    //EventsManager::Add(this, Event::Type::JoyDeviceRemoved);
}

InputDevice::~InputDevice()
{
}

Gamepad& InputDevice::GetController(int32_t device)
{
    if (_controllers.count(device) == 0) {
        _controllers[device].reset(new Gamepad(device));
    }
    return *_controllers[device];
}

void InputDevice::RemoveController(int32_t i)
{
    _controllers.erase(i);
}

Signal<const Event::GameControllerAxis&>& InputDevice::OnAxis(uint8_t index, const GameController::Axis& axis)
{
    return GetController(index).onAxis.at(size_t(axis));
}

Signal<const Event::GameControllerButton&>& InputDevice::OnButton(uint8_t index, const GameController::Button& button)
{
    return GetController(index).onButton.at(size_t(button));
}

Signal<const Event::GameControllerButton&>& InputDevice::OnButtonUp(uint8_t index, const GameController::Button& button)
{
    return GetController(index).onButtonUp.at(size_t(button));
}

Signal<const Event::GameControllerButton&>& InputDevice::OnButtonDown(uint8_t index, const GameController::Button& button)
{
    return GetController(index).onButtonDown.at(size_t(button));
}

Signal<const Event::GameControllerDevice&>& InputDevice::OnConnection(uint8_t index)
{
    return GetController(index).onConnection;
}

Signal<const Event::GameControllerDevice&>& InputDevice::OnDisconnection(uint8_t index)
{
    return GetController(index).onDisconnection;
}

void InputDevice::Rumble(uint8_t index, float strength, int duration)
{
    return GetController(index).Rumble(strength, duration);
}

float InputDevice::GetAxis(uint8_t index, GameController::Axis axis)
{
    return GetController(index).GetAxis(axis);
}

bool InputDevice::GetButton(uint8_t index, GameController::Button button)
{
    return GetController(index).GetButton(button);
}

int InputDevice::_GetControllerIndex(int32_t device)
{
    for (auto& controller : _controllers) {
        if (controller.second->Is(device))
            return (controller.first);
    }
    return (-1);
}

void InputDevice::_ProcessEvent(const Event& event)
{
    switch (event.type) {
    case Event::Type::JoyAxisMotion:
        break;
    case Event::Type::JoyBallMotion:
        break;
    case Event::Type::JoyHatMotion:
        break;
    case Event::Type::JoyButtonDown:
        break;
    case Event::Type::JoyButtonUp:
        break;
    case Event::Type::JoyDeviceAdded:
        break;
    case Event::Type::JoyDeviceRemoved:
        break;
    case Event::Type::ControllerAxisMotion: {
        auto& axisEvent{ event.Get<Event::GameControllerAxis>() };
        auto index{ _GetControllerIndex(axisEvent.id) };
        GetController(index).onAxis.at(size_t(axisEvent.axis))(axisEvent);
        break;
    }
    case Event::Type::ControllerButtonDown: {
        auto& buttonEvent{ event.Get<Event::GameControllerButton>() };
        auto index{ _GetControllerIndex(buttonEvent.id) };
        GetController(index).onButton.at(size_t(buttonEvent.button))(buttonEvent);
        GetController(index).onButtonDown.at(size_t(buttonEvent.button))(buttonEvent);
        break;
    }
    case Event::Type::ControllerButtonUp: {
        auto& buttonEvent{ event.Get<Event::GameControllerButton>() };
        auto index{ _GetControllerIndex(buttonEvent.id) };
        GetController(index).onButton.at(size_t(buttonEvent.button))(buttonEvent);
        GetController(index).onButtonUp.at(size_t(buttonEvent.button))(buttonEvent);
        break;
    }
    case Event::Type::ControllerDeviceAdded: {
        auto& buttonEvent{ event.Get<Event::GameControllerDevice>() };
        auto index{ buttonEvent.id };
        GetController(index).onConnection(buttonEvent);
        break;
    }
    case Event::Type::ControllerDeviceRemoved: {
        auto& buttonEvent{ event.Get<Event::GameControllerDevice>() };
        auto index{ _GetControllerIndex(buttonEvent.id) };
        GetController(index).onDisconnection(buttonEvent);
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