/*
* @Author: gpinchon
* @Date:   2021-05-19 23:30:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 12:13:52
*/

#include <Driver/SDL2/EventsManager.hpp>
#include <Driver/SDL2/Event.hpp>
#include <Engine.hpp>

#include <SDL.h>
#include <SDL_events.h>

EventsManager::Impl::Impl()
{
    if (!SDL_WasInit(SDL_INIT_EVENTS) && SDL_Init(SDL_INIT_EVENTS) < 0)
        throw std::runtime_error(SDL_GetError());
}

void EventsManager::Impl::PollEvents()
{
    //secure current custom events and clear vector
    _lock.lock();
    auto customEvents{ _customEvents };
    _customEvents.clear();
    _lock.unlock();
    SDL_Event SDLevent;
    while (SDL_PollEvent(&SDLevent)) {
        auto event = SDL2::CreateEvent(&SDLevent);
        On(event.type)(event);
    }
    for (const auto &event : customEvents) {
        On(event.type)(event);
    }
}
