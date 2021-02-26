/*
* @Author: gpinchon
* @Date:   2020-08-27 18:48:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:42:31
*/

#include "Tools/Tools.hpp"
#include <SDL_events.h>
#include <iostream>
#include <glm/glm.hpp>

using namespace Tools;

void PrintEventName(SDL_Event* event)
{
    switch (event->type) {
    case SDL_FIRSTEVENT: {
        std::cout << "Event : SDL_FIRSTEVENT" << std::endl;
        break;
    }
    case SDL_QUIT: {
        std::cout << "Event : SDL_QUIT" << std::endl;
        break;
    }
    case SDL_APP_TERMINATING: {
        std::cout << "Event : SDL_APP_TERMINATING" << std::endl;
        break;
    }
    case SDL_APP_LOWMEMORY: {
        std::cout << "Event : SDL_APP_LOWMEMORY" << std::endl;
        break;
    }
    case SDL_APP_WILLENTERBACKGROUND: {
        std::cout << "Event : SDL_APP_WILLENTERBACKGROUND" << std::endl;
        break;
    }
    case SDL_APP_DIDENTERBACKGROUND: {
        std::cout << "Event : SDL_APP_DIDENTERBACKGROUND" << std::endl;
        break;
    }
    case SDL_APP_WILLENTERFOREGROUND: {
        std::cout << "Event : SDL_APP_WILLENTERFOREGROUND" << std::endl;
        break;
    }
    case SDL_APP_DIDENTERFOREGROUND: {
        std::cout << "Event : SDL_APP_DIDENTERFOREGROUND" << std::endl;
        break;
    }
    case SDL_WINDOWEVENT: {
        std::cout << "Event : SDL_WINDOWEVENT" << std::endl;
        break;
    }
    case SDL_SYSWMEVENT: {
        std::cout << "Event : SDL_SYSWMEVENT" << std::endl;
        break;
    }
    case SDL_KEYDOWN: {
        std::cout << "Event : SDL_KEYDOWN" << std::endl;
        break;
    }
    case SDL_KEYUP: {
        std::cout << "Event : SDL_KEYUP" << std::endl;
        break;
    }
    case SDL_TEXTEDITING: {
        std::cout << "Event : SDL_TEXTEDITING" << std::endl;
        break;
    }
    case SDL_TEXTINPUT: {
        std::cout << "Event : SDL_TEXTINPUT" << std::endl;
        break;
    }
    case SDL_KEYMAPCHANGED: {
        std::cout << "Event : SDL_KEYMAPCHANGED" << std::endl;
        break;
    }
    case SDL_MOUSEMOTION: {
        std::cout << "Event : SDL_MOUSEMOTION" << std::endl;
        break;
    }
    case SDL_MOUSEBUTTONDOWN: {
        std::cout << "Event : SDL_MOUSEBUTTONDOWN" << std::endl;
        break;
    }
    case SDL_MOUSEBUTTONUP: {
        std::cout << "Event : SDL_MOUSEBUTTONUP" << std::endl;
        break;
    }
    case SDL_MOUSEWHEEL: {
        std::cout << "Event : SDL_MOUSEWHEEL" << std::endl;
        break;
    }
    case SDL_JOYAXISMOTION: {
        std::cout << "Event : SDL_JOYAXISMOTION" << std::endl;
        break;
    }
    case SDL_JOYBALLMOTION: {
        std::cout << "Event : SDL_JOYBALLMOTION" << std::endl;
        break;
    }
    case SDL_JOYHATMOTION: {
        std::cout << "Event : SDL_JOYHATMOTION" << std::endl;
        break;
    }
    case SDL_JOYBUTTONDOWN: {
        std::cout << "Event : SDL_JOYBUTTONDOWN" << std::endl;
        break;
    }
    case SDL_JOYBUTTONUP: {
        std::cout << "Event : SDL_JOYBUTTONUP" << std::endl;
        break;
    }
    case SDL_JOYDEVICEADDED: {
        std::cout << "Event : SDL_JOYDEVICEADDED" << std::endl;
        break;
    }
    case SDL_JOYDEVICEREMOVED: {
        std::cout << "Event : SDL_JOYDEVICEREMOVED" << std::endl;
        break;
    }
    case SDL_CONTROLLERAXISMOTION: {
        std::cout << "Event : SDL_CONTROLLERAXISMOTION" << std::endl;
        break;
    }
    case SDL_CONTROLLERBUTTONDOWN: {
        std::cout << "Event : SDL_CONTROLLERBUTTONDOWN" << std::endl;
        break;
    }
    case SDL_CONTROLLERBUTTONUP: {
        std::cout << "Event : SDL_CONTROLLERBUTTONUP" << std::endl;
        break;
    }
    case SDL_CONTROLLERDEVICEADDED: {
        std::cout << "Event : SDL_CONTROLLERDEVICEADDED" << std::endl;
        break;
    }
    case SDL_CONTROLLERDEVICEREMOVED: {
        std::cout << "Event : SDL_CONTROLLERDEVICEREMOVED" << std::endl;
        break;
    }
    case SDL_CONTROLLERDEVICEREMAPPED: {
        std::cout << "Event : SDL_CONTROLLERDEVICEREMAPPED" << std::endl;
        break;
    }
    case SDL_FINGERDOWN: {
        std::cout << "Event : SDL_FINGERDOWN" << std::endl;
        break;
    }
    case SDL_FINGERUP: {
        std::cout << "Event : SDL_FINGERUP" << std::endl;
        break;
    }
    case SDL_FINGERMOTION: {
        std::cout << "Event : SDL_FINGERMOTION" << std::endl;
        break;
    }
    case SDL_DOLLARGESTURE: {
        std::cout << "Event : SDL_DOLLARGESTURE" << std::endl;
        break;
    }
    case SDL_DOLLARRECORD: {
        std::cout << "Event : SDL_DOLLARRECORD" << std::endl;
        break;
    }
    case SDL_MULTIGESTURE: {
        std::cout << "Event : SDL_MULTIGESTURE" << std::endl;
        break;
    }
    case SDL_CLIPBOARDUPDATE: {
        std::cout << "Event : SDL_CLIPBOARDUPDATE" << std::endl;
        break;
    }
    case SDL_DROPFILE: {
        std::cout << "Event : SDL_DROPFILE" << std::endl;
        break;
    }
    case SDL_DROPTEXT: {
        std::cout << "Event : SDL_DROPTEXT" << std::endl;
        break;
    }
    case SDL_DROPBEGIN: {
        std::cout << "Event : SDL_DROPBEGIN" << std::endl;
        break;
    }
    case SDL_DROPCOMPLETE: {
        std::cout << "Event : SDL_DROPCOMPLETE" << std::endl;
        break;
    }
    case SDL_AUDIODEVICEADDED: {
        std::cout << "Event : SDL_AUDIODEVICEADDED" << std::endl;
        break;
    }
    case SDL_AUDIODEVICEREMOVED: {
        std::cout << "Event : SDL_AUDIODEVICEREMOVED" << std::endl;
        break;
    }
    case SDL_RENDER_TARGETS_RESET: {
        std::cout << "Event : SDL_RENDER_TARGETS_RESET" << std::endl;
        break;
    }
    case SDL_RENDER_DEVICE_RESET: {
        std::cout << "Event : SDL_RENDER_DEVICE_RESET" << std::endl;
        break;
    }
    case SDL_USEREVENT: {
        std::cout << "Event : SDL_USEREVENT" << std::endl;
        break;
    }
    case SDL_LASTEVENT: {
        std::cout << "Event : SDL_LASTEVENT" << std::endl;
        break;
    }
    }
}
namespace Tools {
    float Halton(int b, int i)
    {
        float r = 0.0;
        float f = 1.0;
        while (i > 0) {
            f = f / float(b);
            r = r + f * float(i % b);
            i = int(floor(float(i) / float(b)));
        }
        return r;
    }

    float Halton2(int i)
    {
        return Halton(2, i);
    }

    glm::vec2 Halton23(int i)
    {
        return glm::vec2(Halton2(i), Halton(3, i));
    }
}
