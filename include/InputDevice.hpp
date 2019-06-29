/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:28:07
*/

#pragma once

#include <SDL2/SDL.h>

class InputDevice {
public:
    virtual void process_event(SDL_Event*) = 0;

protected:
    InputDevice() = default;
};