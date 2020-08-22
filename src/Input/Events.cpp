/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:03:44
*/

#include "Input/Events.hpp"
#include "Input/InputDevice.hpp" // for InputDevice
#include "Callback.hpp"
#include "Engine.hpp" // for Stop
#include <SDL_timer.h> // for SDL_GetTicks
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

void Events::AddRefreshCallback(std::shared_ptr<Callback<void()>> callback)
{
    if (callback != nullptr)
        _get()._rcallbacks.push_back(callback);
}

/*void Events::AddRefreshCallback(t_callback callback, void *argument)
{
    _get()._rcallbacks.push_back(std::pair(callback, argument));
}*/

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

#include "Debug.hpp"

int Events::refresh()
{
    double ticks;
    static double last_ticks;

    ticks = SDL_GetTicks() / 1000.f;
    _get()._delta_time = ticks - last_ticks;
    last_ticks = ticks;
    for (const auto &callback : _get()._rcallbacks) {
        if (callback != nullptr)
            (*callback)();
    }
    SDL_Event event;
    event.type = EVENT_REFRESH;
    filter(nullptr, &event);
    return (0);
}

void Events::RemoveRefreshCallback(std::shared_ptr<Callback<void()>> callback)
{
    _get()._rcallbacks.erase(std::remove(_get()._rcallbacks.begin(), _get()._rcallbacks.end(), callback), _get()._rcallbacks.end());
    //TODO : Enable this when C++20 is out
    //std::erase_if(_get()._rcallbacks, [callback](auto it) { return it->second == reinterpret_cast<size_t>(&callback); });
}
