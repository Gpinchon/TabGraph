/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:28:16
*/

#pragma once

#include "GLIncludes.hpp"
#include <SDL2/SDL.h>
#include <map>
#include <set>

#define EVENT_REFRESH 0

class InputDevice;

typedef void (*t_callback)(SDL_Event* event);

class Events {
public:
    static void add(InputDevice* device, SDL_EventType event_type);
    static void remove(InputDevice* device, SDL_EventType event_type);
    static int filter(void* e, SDL_Event* event);
    static int refresh(void);
    static void set_refresh_callback(t_callback callback);
    static double delta_time();

private:
    static void window(SDL_Event* event);
    static Events& _get();
    static Events* _instance;
    double _delta_time {0};
    t_callback _rcallback{ nullptr };
    std::map<Uint32, std::set<InputDevice*>> _input_devices;
    Events();
};
