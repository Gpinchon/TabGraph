/*
* @Author: gpinchon
* @Date:   2021-05-20 12:12:41
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 16:02:51
*/

#include <Event/Event.hpp>

union SDL_Event;

namespace SDL2 {
Event::Type GetEventType(uint32_t eventType);
Event CreateEvent(const SDL_Event* event);
};