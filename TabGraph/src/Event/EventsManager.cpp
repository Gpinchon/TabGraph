/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 12:12:11
*/

#include <Engine.hpp>
#include <Event/Event.hpp>
#include <Event/EventsManager.hpp>

#if MEDIALIBRARY == SDL2
#include <Driver/SDL2/EventsManager.hpp>
#endif //MEDIALIBRARY == SDL2

EventsManager::EventsManager()
    : _impl(new EventsManager::Impl)
{
}

EventsManager& EventsManager::_Get()
{
    static EventsManager EventsManager;
    return EventsManager;
}

Signal<const Event&>& EventsManager::On(Event::Type type)
{
    return _Get()._impl->On(type);
}

void EventsManager::PushEvent(const Event& event)
{
    return _Get()._impl->PushEvent(event);
}

void EventsManager::PollEvents()
{
    return _Get()._impl->PollEvents();
}
