/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:28:01
*/

#pragma once

//#include "GLIncludes.hpp"
#include <SDL2/SDL.h>

class InputDevice {
public:
    virtual void process_event(SDL_Event*) = 0;

protected:
    InputDevice() = default;
};