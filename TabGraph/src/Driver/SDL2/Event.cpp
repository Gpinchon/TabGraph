/*
* @Author: gpinchon
* @Date:   2021-05-20 12:13:34
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 12:39:43
*/

#include <Driver/SDL2/Event.hpp>
#include <Driver/SDL2/GameController.hpp>
#include <Driver/SDL2/Keyboard.hpp>
#include <Driver/SDL2/Mouse.hpp>
#include <Driver/SDL2/Window.hpp>

#include <SDL_events.h>

namespace SDL2 {
const static std::map<SDL_EventType, Event::Type> s_eventTypeLUT {
    { SDL_WINDOWEVENT, Event::Type::WindowEvent },
    { SDL_MOUSEMOTION, Event::Type::MouseMotion },
    { SDL_MOUSEBUTTONDOWN, Event::Type::MouseButtonDown },
    { SDL_MOUSEBUTTONUP, Event::Type::MouseButtonUp },
    { SDL_MOUSEWHEEL, Event::Type::MouseWheel },
    { SDL_JOYAXISMOTION, Event::Type::JoyAxisMotion },
    { SDL_JOYBALLMOTION, Event::Type::JoyBallMotion },
    { SDL_JOYHATMOTION, Event::Type::JoyHatMotion },
    { SDL_JOYBUTTONDOWN, Event::Type::JoyButtonDown },
    { SDL_JOYBUTTONUP, Event::Type::JoyButtonUp },
    { SDL_JOYDEVICEADDED, Event::Type::JoyDeviceAdded },
    { SDL_JOYDEVICEREMOVED, Event::Type::JoyDeviceRemoved },
    { SDL_CONTROLLERAXISMOTION, Event::Type::ControllerAxisMotion },
    { SDL_CONTROLLERBUTTONDOWN, Event::Type::ControllerButtonDown },
    { SDL_CONTROLLERBUTTONUP, Event::Type::ControllerButtonUp },
    { SDL_CONTROLLERDEVICEADDED, Event::Type::ControllerDeviceAdded },
    { SDL_CONTROLLERDEVICEREMOVED, Event::Type::ControllerDeviceRemoved },
    { SDL_CONTROLLERDEVICEREMAPPED, Event::Type::ControllerDeviceRemapped },
    { SDL_CONTROLLERTOUCHPADDOWN, Event::Type::ControllerTouchpadDown },
    { SDL_CONTROLLERTOUCHPADMOTION, Event::Type::ControllerTouchpadMotion },
    { SDL_CONTROLLERTOUCHPADUP, Event::Type::ControllerTouchpadUp },
    { SDL_CONTROLLERSENSORUPDATE, Event::Type::ControllerSensorUpdate },
    { SDL_QUIT, Event::Type::Quit },
    { SDL_KEYDOWN, Event::Type::KeyDown },
    { SDL_KEYUP, Event::Type::KeyUp },
    { SDL_TEXTEDITING, Event::Type::TextEditing },
    { SDL_TEXTINPUT, Event::Type::TextInput },
    { SDL_LASTEVENT, Event::Type::MaxValue }
};

Event::Type GetEventType(uint32_t type)
{
    return s_eventTypeLUT.at(SDL_EventType(type));
};

Event CreateEvent(const SDL_Event* SDLevent)
{
    Event event;
    event.type = GetEventType(SDLevent->type);
    switch (SDLevent->type) {
    case SDL_QUIT:
        break;
    case SDL_APP_TERMINATING:
        break;
    case SDL_APP_LOWMEMORY:
        break;
    case SDL_APP_WILLENTERBACKGROUND:
        break;
    case SDL_APP_DIDENTERBACKGROUND:
        break;
    case SDL_APP_WILLENTERFOREGROUND:
        break;
    case SDL_APP_DIDENTERFOREGROUND:
        break;
    case SDL_LOCALECHANGED:
        break;
    case SDL_DISPLAYEVENT:
        break;
    case SDL_WINDOWEVENT:
        event.data = SDL2::Window::CreateEventData(SDLevent->window);
        break;
    case SDL_SYSWMEVENT:
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        event.data = SDL2::Keyboard::CreateEventData(SDLevent->key);
        break;
    case SDL_TEXTEDITING:
        event.data = SDL2::Keyboard::CreateEventData(SDLevent->edit);
        break;
    case SDL_TEXTINPUT:
        event.data = SDL2::Keyboard::CreateEventData(SDLevent->text);
        break;
    case SDL_KEYMAPCHANGED:
        break;
    case SDL_MOUSEMOTION:
        event.data = SDL2::Mouse::CreateEventData(SDLevent->motion);
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        event.data = SDL2::Mouse::CreateEventData(SDLevent->button);
        break;
    case SDL_MOUSEWHEEL:
        event.data = SDL2::Mouse::CreateEventData(SDLevent->wheel);
        break;
    case SDL_JOYAXISMOTION:
        break;
    case SDL_JOYBALLMOTION:
        break;
    case SDL_JOYHATMOTION:
        break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        break;
    case SDL_JOYDEVICEADDED:
        break;
    case SDL_JOYDEVICEREMOVED:
        break;
    case SDL_CONTROLLERAXISMOTION:
        event.data = SDL2::GameController::CreateEventData(SDLevent->caxis);
        break;
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        event.data = SDL2::GameController::CreateEventData(SDLevent->cbutton);
        break;
    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
        event.data = SDL2::GameController::CreateEventData(SDLevent->cdevice);
        break;
    case SDL_CONTROLLERDEVICEREMAPPED:
        break;
    case SDL_CONTROLLERTOUCHPADDOWN:
        break;
    case SDL_CONTROLLERTOUCHPADMOTION:
        break;
    case SDL_CONTROLLERTOUCHPADUP:
        break;
    case SDL_CONTROLLERSENSORUPDATE:
        break;
    case SDL_FINGERDOWN:
        break;
    case SDL_FINGERUP:
        break;
    case SDL_FINGERMOTION:
        break;
    case SDL_DOLLARGESTURE:
        break;
    case SDL_DOLLARRECORD:
        break;
    case SDL_MULTIGESTURE:
        break;
    case SDL_CLIPBOARDUPDATE:
        break;
    case SDL_DROPFILE:
        break;
    case SDL_DROPTEXT:
        break;
    case SDL_DROPBEGIN:
        break;
    case SDL_DROPCOMPLETE:
        break;
    case SDL_AUDIODEVICEADDED:
        break;
    case SDL_AUDIODEVICEREMOVED:
        break;
    case SDL_SENSORUPDATE:
        break;
    case SDL_RENDER_TARGETS_RESET:
        break;
    case SDL_RENDER_DEVICE_RESET:
        break;
    default:
        throw std::runtime_error("Unknown Event::Type");
    }
    return event;
};
};
