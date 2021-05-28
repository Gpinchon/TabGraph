/*
* @Author: gpinchon
* @Date:   2021-05-19 23:30:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 12:13:52
*/

#include <Driver/SDL2/EventsManager.hpp>
#include <Driver/SDL2/Event.hpp>
#include <Engine.hpp>

#include <SDL_events.h>

void EventsManager::Impl::PollEvents()
{
    _lock.lock();
    SDL_Event SDLevent;
    while (SDL_PollEvent(&SDLevent)) {
        auto event = SDL2::CreateEvent(&SDLevent);
        On(event.type)(event);
    }
    while (!_customEvents.empty()) {
        auto event{ std::move(_customEvents.front()) };
        _customEvents.pop();
        On(event.type)(event);
    }
    _lock.unlock();
}
