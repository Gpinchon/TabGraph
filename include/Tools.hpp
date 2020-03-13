/*
* @Author: gpi
* @Date:   2019-06-28 13:27:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:59:06
*/

#pragma once

#ifndef M_PI
#define M_PI 3.14159265359f
#endif // M_PI

#define CYCLE(nbr, min, max) (nbr >= max ? min : nbr <= min ? max : nbr)

union SDL_Event;

namespace Tools {
void PrintEventName(SDL_Event*);
}