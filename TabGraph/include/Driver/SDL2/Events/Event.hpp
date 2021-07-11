/*
* @Author: gpinchon
* @Date:   2021-05-20 12:12:41
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:53
*/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Events/Event.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
union SDL_Event;

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace SDL2 {
TabGraph::Events::Event::Type GetEventType(uint32_t eventType);
TabGraph::Events::Event CreateEvent(const SDL_Event* event);
};