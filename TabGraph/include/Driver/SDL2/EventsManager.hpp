/*
* @Author: gpinchon
* @Date:   2021-05-19 23:28:33
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 12:13:45
*/
#pragma once

#include <Event/Event.hpp>
#include <Event/EventsManager.hpp>
#include <Event/InputDevice/InputDevice.hpp>

#include <map>
#include <set>

union SDL_Event;

class EventsManager::Impl {
public:
    inline void Add(InputDevice* device, Event::Type event_type) {
        _trackablePointees[event_type][device] = device->GetWeakPtr();
        _inputDevices[event_type].insert(device);
    }
    inline void Remove(InputDevice* device, Event::Type eventType) {
        auto inputDevices = _inputDevices.find(eventType);
        if (inputDevices != _inputDevices.end()) {
            inputDevices->second.erase(device);
            _trackablePointees.at(eventType).erase(device);
        }
    }
    void PollEvents();

private:
    std::unordered_map<Event::Type, std::unordered_map<InputDevice*, std::weak_ptr<TrackablePointee>>> _trackablePointees;
    std::unordered_map<Event::Type, std::set<InputDevice*>> _inputDevices;
    static int FilterEvent(void* data, SDL_Event* event);
};
