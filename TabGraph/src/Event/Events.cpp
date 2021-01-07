/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:03:44
*/

#include "Event/Events.hpp"
#include "Event/InputDevice.hpp" // for InputDevice
#include "Engine.hpp" // for Stop
#include <SDL_timer.h> // for SDL_GetTicks
#include <utility> // for pair


Events::Events()
{
    Engine::OnFixedUpdate().ConnectMember(this, &Events::_Refresh);
    SDL_SetEventFilter(_Filter, nullptr);
}

Events& Events::_Get()
{
    static Events events;
    return events;
}

void Events::Add(InputDevice* device, SDL_EventType event_type)
{
    _Get()._input_devices[event_type].insert(device);
}

void Events::Remove(InputDevice* device, SDL_EventType event_type)
{
    auto inputDevices = _Get()._input_devices.find(event_type);
    if (inputDevices != _Get()._input_devices.end())
        inputDevices->second.erase(device);
}

Signal<float>& Events::OnRefresh()
{
    return _Get()._onRefresh;
}

int Events::_Filter(void* /*unused*/, SDL_Event* event)
{
    if (event->type == SDL_QUIT) {
        Engine::Stop();
    }
    auto inputdevice = _Get()._input_devices.find(event->type);
    if (inputdevice != _Get()._input_devices.end()) {
        for (auto device : inputdevice->second)
            device->process_event(event);
    }
    return (0);
}

#include "Debug.hpp"

void Events::_Refresh(float delta)
{
    _onRefresh(delta);
    SDL_Event event;
    event.type = EVENT_REFRESH;
    _Filter(nullptr, &event);
}
