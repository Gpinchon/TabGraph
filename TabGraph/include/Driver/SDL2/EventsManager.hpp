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
#include <map>
#include <queue>
#include <mutex>

union SDL_Event;

class EventsManager::Impl {
public:
    inline Signal<const Event&>& On(Event::Type type) {
        return _onEvent.at(size_t(type));
    }
    inline void PushEvent(const Event& event) {
        _lock.lock();
        _customEvents.push(event);
        _lock.unlock();
    }
    void PollEvents();

private:
    std::array<Signal<const Event&>, size_t(Event::Type::MaxValue)> _onEvent;
    std::queue<Event> _customEvents;
    std::mutex _lock;
};
