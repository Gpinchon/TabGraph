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
        TaskComplete,
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
    /**
     * @brief emitted when a task dispatched through a DispatchQueue gets completed.
    */
    struct TaskComplete {
        uint16_t dispatchQueueID;
        uint32_t taskID;
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
        Event::Asset,
        Event::TaskComplete>
        data;
    template <typename T>
    const auto& Get() const
    {
        return std::get<T>(data);
    }
};
