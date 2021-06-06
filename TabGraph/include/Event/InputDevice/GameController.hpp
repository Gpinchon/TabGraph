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

#include <glm/fwd.hpp>

namespace GameController {
Signal<const Event::GameControllerAxis&>& OnAxis(uint8_t index, const Axis&);
Signal<const Event::GameControllerButton&>& OnButton(uint8_t index, const Button&);
Signal<const Event::GameControllerButton&>& OnButtonUp(uint8_t index, const Button&);
Signal<const Event::GameControllerButton&>& OnButtonDown(uint8_t index, const Button&);
Signal<const Event::GameControllerDevice&>& OnConnection(uint8_t index);
Signal<const Event::GameControllerDevice&>& OnDisconnection(uint8_t index);
/**
 * @brief Rumbles the entire game controller
 * @param index : the index of the controller
 * @param lowFrequency : low frequency vibrations intensity [0 1]
 * @param highFrequency : high frequency vibrations intensity [0 1]
 * @param duration : vibration duration in milliseconds
 * @return : true if supported, false otherwise
*/
bool Rumble(uint8_t index, float lowFrequency, float highFrequency, int duration);
/**
 * @brief Rumbles the triggers
 * @param index : the index of the controller 
 * @param left_rumble : left stick vibrations intensity [0 1]
 * @param right_rumble : right stick vibrations intensity [0 1]
 * @param duration : vibration duration in milliseconds 
 * @return : true if supported, false otherwise
*/
bool RumbleTriggers(uint8_t index, float left_rumble, float right_rumble, uint32_t duration_ms);
bool SetLED(uint8_t index, const glm::vec3& color);
float GetAxis(uint8_t index, GameController::Axis);
bool GetButton(uint8_t index, GameController::Button);
}