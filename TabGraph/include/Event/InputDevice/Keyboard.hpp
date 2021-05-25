/*
* @Author: gpinchon
* @Date:   2021-05-20 17:49:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 17:51:31
*/
#pragma once

#include <Event/InputDevice/InputDevice.hpp>
#include <Event/Event.hpp>
#include <Event/Keyboard.hpp>
#include <Event/Signal.hpp>

namespace Keyboard {
/**
* @brief Gets the Key status
* @param key : the Key to query the status
* @return true if Key is currently down
*/
bool GetKeyState(Keyboard::Key);
/**
* @brief will be triggered on key both key press and release
* @param key the key to get the Signal for
* @return a reference to the Signal attached to this key
*/
Signal<const Event::Keyboard&>& OnKey(Keyboard::Key key);
/**
* @brief will be triggered on key release
*/
Signal<const Event::Keyboard&>& OnKeyUp(Keyboard::Key key);
/**
* @brief will be triggered on key press
*/
Signal<const Event::Keyboard&>& OnKeyDown(Keyboard::Key key);
}