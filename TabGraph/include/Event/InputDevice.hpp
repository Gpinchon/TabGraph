/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:29
*/

#pragma once

#include <SDL.h>

class InputDevice {
public:
    virtual void process_event(SDL_Event*) = 0;

protected:
    InputDevice() = default;
};