/*
* @Author: gpinchon
* @Date:   2021-05-19 23:30:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 12:13:52
*/

#include <Driver/SDL2/EventsManager.hpp>
#include <Driver/SDL2/Event.hpp>
#include <Engine.hpp>
#include <Event/InputDevice/InputDevice.hpp>

#include <SDL_events.h>

void EventsManager::Impl::PollEvents()
{
    SDL_Event SDLevent;
    while (SDL_PollEvent(&SDLevent)) {
        auto event = SDL2::CreateEvent(&SDLevent);
        auto inputdevice = _inputDevices.find(event.type);
        if (inputdevice != _inputDevices.end()) {
            for (auto& device : inputdevice->second) {
                if (_trackablePointees.at(event.type).at(device).expired())
                    Remove(device, event.type);
                else {
                    if (device != nullptr)
                        device->ProcessEvent(event);
                }
            }
        }
    }
}
