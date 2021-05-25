/*
* @Author: gpinchon
* @Date:   2021-05-20 19:11:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-21 12:48:41
*/

#pragma once

#include <Event/Event.hpp>
#include <Event/GameController.hpp>
#include <Event/Signal.hpp>

namespace GameController {
Signal<const Event::GameControllerAxis&>& OnAxis(uint8_t index, const Axis&);
Signal<const Event::GameControllerButton&>& OnButton(uint8_t index, const Button&);
Signal<const Event::GameControllerButton&>& OnButtonUp(uint8_t index, const Button&);
Signal<const Event::GameControllerButton&>& OnButtonDown(uint8_t index, const Button&);
Signal<const Event::GameControllerDevice&>& OnConnection(uint8_t index);
Signal<const Event::GameControllerDevice&>& OnDisconnection(uint8_t index);
void Rumble(uint8_t index, float strength, int duration);
float GetAxis(uint8_t index, GameController::Axis);
bool GetButton(uint8_t index, GameController::Button);
}