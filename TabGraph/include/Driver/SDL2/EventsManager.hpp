/*
* @Author: gpinchon
* @Date:   2021-05-19 23:28:33
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 12:13:45
*/
#pragma once

#include <Event/Event.hpp>
#include <Event/EventsManager.hpp>
#include <Event/Signal.hpp>

#include <array>
#include <queue>
#include <mutex>

union SDL_Event;

class EventsManager::Impl {
public:
    Impl();
    inline Signal<const Event&>& On(Event::Type type) {
        return _onEvent.at(size_t(type));
    }
    inline void PushEvent(const Event& event) {
        std::unique_lock lock(_lock);
        _eventsQueue.push(event);
    }
    void PollEvents();

private:
    std::array<Signal<const Event&>, size_t(Event::Type::MaxValue)> _onEvent;
    std::queue<Event> _eventsQueue;
    std::mutex _lock;
};
