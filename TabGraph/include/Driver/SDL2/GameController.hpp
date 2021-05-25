/*
* @Author: gpinchon
* @Date:   2021-05-21 11:51:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-21 12:49:50
*/

#pragma once

#include <Event/Event.hpp>
#include <Event/GameController.hpp>
#include <Event/InputDevice/InputDevice.hpp>
#include <Event/Signal.hpp>

#include <map>

namespace GameController {
class Gamepad;
struct InputDevice : public ::InputDevice {
    InputDevice();
    ~InputDevice();
    Gamepad& GetController(int32_t index);
    void RemoveController(int32_t index);
    virtual void ProcessEvent(const Event& event) override;
    Signal<const Event::GameControllerAxis&>& OnAxis(uint8_t index, const GameController::Axis&);
    Signal<const Event::GameControllerButton&>& OnButton(uint8_t index, const GameController::Button&);
    Signal<const Event::GameControllerButton&>& OnButtonUp(uint8_t index, const GameController::Button&);
    Signal<const Event::GameControllerButton&>& OnButtonDown(uint8_t index, const GameController::Button&);
    Signal<const Event::GameControllerDevice&>& OnConnection(uint8_t index);
    Signal<const Event::GameControllerDevice&>& OnDisconnection(uint8_t index);
    void Rumble(uint8_t index, float strength, int duration);
    float GetAxis(uint8_t index, GameController::Axis);
    bool GetButton(uint8_t index, GameController::Button);

private:
    int _GetControllerIndex(int32_t device);
    std::map<int32_t, std::unique_ptr<Gamepad>> _controllers;
};
};

struct SDL_ControllerAxisEvent;
struct SDL_ControllerButtonEvent;
struct SDL_ControllerDeviceEvent;

namespace SDL2 {
namespace GameController {
    Event::GameControllerAxis CreateEventData(const SDL_ControllerAxisEvent& event);
    Event::GameControllerButton CreateEventData(const SDL_ControllerButtonEvent& event);
    Event::GameControllerDevice CreateEventData(const SDL_ControllerDeviceEvent& event);
};
};
