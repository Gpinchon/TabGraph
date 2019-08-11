/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:21:45
*/

#include "Events.hpp"
#include "Engine.hpp" // for Stop
#include "InputDevice.hpp" // for InputDevice
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <iostream> // for operator<<, endl, basic_ostream, cout
#include <utility> // for pair

Events* Events::_instance = nullptr;

Events::Events()
{
}

Events& Events::_get()
{
    if (_instance == nullptr)
        _instance = new Events();
    return (*_instance);
}

double Events::delta_time()
{
    return (_get()._delta_time);
}

void Events::Add(InputDevice* device, SDL_EventType event_type)
{
    _get()._input_devices[event_type].insert(device);
}

void Events::remove(InputDevice* device, SDL_EventType event_type)
{
    auto inputDevices = _get()._input_devices.find(event_type);
    if (inputDevices != _get()._input_devices.end())
        inputDevices->second.erase(device);
}

void Events::set_refresh_callback(t_callback callback)
{
    _get()._rcallback = callback;
}

int Events::filter(void* /*unused*/, SDL_Event* event)
{
    if (event->type == SDL_QUIT) {
        Engine::Stop();
    }
    auto inputdevice = _get()._input_devices.find(event->type);
    if (inputdevice != _get()._input_devices.end()) {
        for (auto device : inputdevice->second)
            device->process_event(event);
    }
    return (0);
}

int Events::refresh()
{
    double ticks;
    static double last_ticks;

    ticks = SDL_GetTicks() / 1000.f;
    _get()._delta_time = ticks - last_ticks;
    last_ticks = ticks;
    if (_get()._rcallback != nullptr) {
        _get()._rcallback(nullptr);
    }
    SDL_Event event;
    event.type = EVENT_REFRESH;
    filter(nullptr, &event);
    return (0);
}
