/*
* @Author: gpinchon
* @Date:   2021-05-20 12:10:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-27 15:16:47
*/

#pragma once

#include <Event/GameController.hpp>
#include <Event/Keyboard.hpp>
#include <Event/Mouse.hpp>

#include <chrono>
#include <glm/vec2.hpp>
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Window;
class Asset;

struct Event {
    enum class Type {
        Unknown = -1,
        //App events
        Quit,
        AppTerminating,
        AppLowMemory,
        AppWillenterBackground,
        AppDidenterBackground,
        AppWillenterForeground,
        AppDidenterForeground,
        LocaleChanged,
        //Window events
        WindowEvent,
        //Keyboard events
        KeyDown,
        KeyUp,
        //Text events
        TextEditing,
        TextInput,
        KeymapChanged,
        //Mouse events
        MouseMotion,
        MouseButtonDown,
        MouseButtonUp,
        MouseWheel,
        //Joystick events
        JoyAxisMotion,
        JoyBallMotion,
        JoyHatMotion,
        JoyButtonDown,
        JoyButtonUp,
        JoyDeviceAdded,
        JoyDeviceRemoved,
        //Game controller events
        ControllerAxisMotion,
        ControllerButtonDown,
        ControllerButtonUp,
        ControllerDeviceAdded,
        ControllerDeviceRemoved,
        ControllerDeviceRemapped,
        ControllerTouchpadDown,
        ControllerTouchpadMotion,
        ControllerTouchpadUp,
        ControllerSensorUpdate,
        //Touch events
        FingerDown,
        FingerUp,
        FingerMotion,
        //Gesture events
        DollarGesture,
        DollarRecord,
        MultiGesture,
        //Clipboard events
        ClipboardUpdate,
        //Drag/Drop events
        DropFile,
        DropText,
        DropBegin,
        DropComplete,
        //Audio device events
        AudioDeviceAdded,
        AudioDeviceremoved,
        //Sensor events
        SensorUpdate,
        //User event for custom events
        User,
        AssetLoaded,
        MaxValue
    };
    struct Empty {
    };
    struct Keyboard {
        std::shared_ptr<::Window> window { nullptr };
        ::Keyboard::Key key { ::Keyboard::Key::Unknown };
        bool state { false }; //true if key down, false if key up
        bool repeat { false }; //true if repeat event
        bool alt { false }; //true if alt is pressed
        bool control { false }; //true if ctrl is pressed
        bool shift { false }; //true if shift is pressed
    };
    struct TextInput {
        std::shared_ptr<::Window> window { nullptr };
        std::string text;
    };
    struct TextEdit : TextInput {
        size_t start { 0 };
        size_t length { 0 };
    };
    struct Mouse {
        std::shared_ptr<::Window> window { nullptr };
        glm::ivec2 position { 0 }; //the Mouse position in pixels
    };
    struct MouseMove : Mouse {
        glm::ivec2 relative { 0 };
    };
    struct MouseButton : Mouse {
        ::Mouse::Button button { ::Mouse::Button::Unknown };
    };
    struct MouseWheel : Mouse {
        ::Mouse::WheelDirection direction; //the scrolling direction (Natural or Flipped)
        glm::ivec2 amount { 0 }; //the scrolling amount in x/y direction
    };
    struct GameController {
        int32_t id { -1 };
    };
    using GameControllerDevice = Event::GameController;
    struct GameControllerAxis : GameController {
        ::GameController::Axis axis { ::GameController::Axis::Unknown };
        float value { 0 };
    };
    struct GameControllerButton : GameController {
        ::GameController::Button button { ::GameController::Button::Unknown };
        bool state { false };
    };
    struct Window {
        enum class Type {
            Unknown = -1,
            Shown,
            Hidden,
            Exposed,
            Moved,
            Resized,
            SizeChanged,
            Minimized,
            Maximized,
            Restored,
            Enter,
            Leave,
            FocusGained,
            FocusLost,
            Close,
            TakeFocus,
            MaxValue
        };
        Type type { Type::Unknown };
        std::shared_ptr<::Window> window { nullptr };
    };
    struct User {
        uint32_t type;
        std::vector<std::byte> data; //user data
    };
    struct Asset {
        bool loaded { false };
        std::shared_ptr<::Asset> asset;
    };
    Type type { Event::Type::Unknown };
    const std::chrono::system_clock::time_point timestamp { std::chrono::system_clock::now() };
    std::variant<
        Event::Empty,
        Event::Keyboard,
        Event::TextInput,
        Event::TextEdit,
        Event::MouseMove,
        Event::MouseButton,
        Event::MouseWheel,
        Event::GameControllerDevice,
        Event::GameControllerAxis,
        Event::GameControllerButton,
        Event::Window,
        Event::User,
        Event::Asset>
        data;
    template <typename T>
    const auto& Get() const
    {
        return std::get<T>(data);
    }
};

inline bool operator<(const Event::Empty& a, const Event::Empty& b)
{
    return false;
}

inline bool operator<(const Event::Keyboard& a, const Event::Keyboard& b)
{
    return a.window < b.window
        && a.key < b.key
        && a.state < b.state
        && a.repeat < b.repeat
        && a.alt < b.alt
        && a.control < b.control
        && a.shift < b.shift;
}

inline bool operator<(const Event::TextInput& a, const Event::TextInput& b)
{
    return a.text < b.text
        && a.window < b.window;
}

inline bool operator<(const Event::TextEdit& a, const Event::TextEdit& b)
{
    return a.length < b.length
        && a.start < b.start
        && a.text < b.text
        && a.window < b.window;
}

#include <glm/vec2.hpp>
#include <glm/vector_relational.hpp>

inline bool operator<(const Event::MouseMove& a, const Event::MouseMove& b)
{
    return glm::all(glm::lessThan(a.position, b.position))
        && glm::all(glm::lessThan(a.relative, b.relative))
        && a.window < b.window;
}

inline bool operator<(const Event::MouseButton& a, const Event::MouseButton& b)
{
    return a.button < b.button
        && glm::all(glm::lessThan(a.position, b.position))
        && a.window < b.window;
}

inline bool operator<(const Event::MouseWheel& a, const Event::MouseWheel& b)
{
    return glm::all(glm::lessThan(a.amount, b.amount))
        && a.direction < b.direction
        && glm::all(glm::lessThan(a.position, b.position))
        && a.window < b.window;
}

inline bool operator<(const Event::GameControllerDevice& a, const Event::GameControllerDevice& b)
{
    return a.id < b.id;
}

inline bool operator<(const Event::GameControllerAxis& a, const Event::GameControllerAxis& b)
{
    return a.axis < b.axis
        && a.id < b.id
        && a.value < b.value;
}

inline bool operator<(const Event::GameControllerButton& a, const Event::GameControllerButton& b)
{
    return a.button < b.button
        && a.id < b.id
        && a.state < b.state;
}

inline bool operator<(const Event::Window& a, const Event::Window& b)
{
    return a.type < b.type
        && a.window < b.window;
}

inline bool operator<(const Event::User& a, const Event::User& b)
{
    return a.data < b.data;
}

inline bool operator<(const Event::Asset& a, const Event::Asset& b)
{
    return a.asset < b.asset;
}

inline bool operator<(const Event& a, const Event& b)
{
    return a.type < b.type
        && a.data < b.data;
}
