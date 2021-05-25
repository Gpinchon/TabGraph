/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:29
*/

#pragma once

#include <Event/Signal.hpp>

struct Event;

struct InputDevice : Trackable {
    virtual void ProcessEvent(const Event&) = 0;
};
