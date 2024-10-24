/*
* @Author: gpinchon
* @Date:   2021-05-20 15:21:41
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 18:25:01
*/

#include <Driver/SDL2/Keyboard.hpp>
#include <Event/EventsManager.hpp>
#include <Window.hpp>

#include <SDL_events.h>
#include <SDL_keyboard.h>

namespace SDL2 {
namespace Keyboard {
    const static std::unordered_map<SDL_Scancode, ::Keyboard::Key> s_keyLUT
    {
        { SDL_SCANCODE_UNKNOWN, ::Keyboard::Key::Unknown },
        { SDL_SCANCODE_A, ::Keyboard::Key::A },
        { SDL_SCANCODE_B, ::Keyboard::Key::B },
        { SDL_SCANCODE_C, ::Keyboard::Key::C },
        { SDL_SCANCODE_D, ::Keyboard::Key::D },
        { SDL_SCANCODE_E, ::Keyboard::Key::E },
        { SDL_SCANCODE_F, ::Keyboard::Key::F },
        { SDL_SCANCODE_G, ::Keyboard::Key::G },
        { SDL_SCANCODE_H, ::Keyboard::Key::H },
        { SDL_SCANCODE_I, ::Keyboard::Key::I },
        { SDL_SCANCODE_J, ::Keyboard::Key::J },
        { SDL_SCANCODE_K, ::Keyboard::Key::K },
        { SDL_SCANCODE_L, ::Keyboard::Key::L },
        { SDL_SCANCODE_M, ::Keyboard::Key::M },
        { SDL_SCANCODE_N, ::Keyboard::Key::N },
        { SDL_SCANCODE_O, ::Keyboard::Key::O },
        { SDL_SCANCODE_P, ::Keyboard::Key::P },
        { SDL_SCANCODE_Q, ::Keyboard::Key::Q },
        { SDL_SCANCODE_R, ::Keyboard::Key::R },
        { SDL_SCANCODE_S, ::Keyboard::Key::S },
        { SDL_SCANCODE_T, ::Keyboard::Key::T },
        { SDL_SCANCODE_U, ::Keyboard::Key::U },
        { SDL_SCANCODE_V, ::Keyboard::Key::V },
        { SDL_SCANCODE_W, ::Keyboard::Key::W },
        { SDL_SCANCODE_X, ::Keyboard::Key::X },
        { SDL_SCANCODE_Y, ::Keyboard::Key::Y },
        { SDL_SCANCODE_Z, ::Keyboard::Key::Z },
        { SDL_SCANCODE_1, ::Keyboard::Key::Num1 },
        { SDL_SCANCODE_2, ::Keyboard::Key::Num2 },
        { SDL_SCANCODE_3, ::Keyboard::Key::Num3 },
        { SDL_SCANCODE_4, ::Keyboard::Key::Num4 },
        { SDL_SCANCODE_5, ::Keyboard::Key::Num5 },
        { SDL_SCANCODE_6, ::Keyboard::Key::Num6 },
        { SDL_SCANCODE_7, ::Keyboard::Key::Num7 },
        { SDL_SCANCODE_8, ::Keyboard::Key::Num8 },
        { SDL_SCANCODE_9, ::Keyboard::Key::Num9 },
        { SDL_SCANCODE_0, ::Keyboard::Key::Num0 },
        { SDL_SCANCODE_RETURN, ::Keyboard::Key::Return },
        { SDL_SCANCODE_ESCAPE, ::Keyboard::Key::Escape },
        { SDL_SCANCODE_BACKSPACE, ::Keyboard::Key::Backspace },
        { SDL_SCANCODE_TAB, ::Keyboard::Key::Tab },
        { SDL_SCANCODE_SPACE, ::Keyboard::Key::Space },
        { SDL_SCANCODE_MINUS, ::Keyboard::Key::Minus },
        { SDL_SCANCODE_EQUALS, ::Keyboard::Key::Equals },
        { SDL_SCANCODE_LEFTBRACKET, ::Keyboard::Key::LeftBracket },
        { SDL_SCANCODE_RIGHTBRACKET, ::Keyboard::Key::RightBracket },
        { SDL_SCANCODE_BACKSLASH, ::Keyboard::Key::Backslash },
        { SDL_SCANCODE_NONUSHASH, ::Keyboard::Key::Nonushash },
        { SDL_SCANCODE_SEMICOLON, ::Keyboard::Key::Semicolon },
        { SDL_SCANCODE_APOSTROPHE, ::Keyboard::Key::Apostrophe },
        { SDL_SCANCODE_GRAVE, ::Keyboard::Key::Grave },
        { SDL_SCANCODE_COMMA, ::Keyboard::Key::Comma },
        { SDL_SCANCODE_PERIOD, ::Keyboard::Key::Period },
        { SDL_SCANCODE_SLASH, ::Keyboard::Key::Slash },
        { SDL_SCANCODE_CAPSLOCK, ::Keyboard::Key::CapsLock },
        { SDL_SCANCODE_F1, ::Keyboard::Key::F1 },
        { SDL_SCANCODE_F2, ::Keyboard::Key::F2 },
        { SDL_SCANCODE_F3, ::Keyboard::Key::F3 },
        { SDL_SCANCODE_F4, ::Keyboard::Key::F4 },
        { SDL_SCANCODE_F5, ::Keyboard::Key::F5 },
        { SDL_SCANCODE_F6, ::Keyboard::Key::F6 },
        { SDL_SCANCODE_F7, ::Keyboard::Key::F7 },
        { SDL_SCANCODE_F8, ::Keyboard::Key::F8 },
        { SDL_SCANCODE_F9, ::Keyboard::Key::F9 },
        { SDL_SCANCODE_F10, ::Keyboard::Key::F10 },
        { SDL_SCANCODE_F11, ::Keyboard::Key::F11 },
        { SDL_SCANCODE_F12, ::Keyboard::Key::F12 },
        { SDL_SCANCODE_PRINTSCREEN, ::Keyboard::Key::PrintScreen },
        { SDL_SCANCODE_SCROLLLOCK, ::Keyboard::Key::ScrollLock },
        { SDL_SCANCODE_PAUSE, ::Keyboard::Key::Pause },
        { SDL_SCANCODE_INSERT, ::Keyboard::Key::Insert },
        { SDL_SCANCODE_HOME, ::Keyboard::Key::Home },
        { SDL_SCANCODE_PAGEUP, ::Keyboard::Key::PageUp },
        { SDL_SCANCODE_DELETE, ::Keyboard::Key::Delete },
        { SDL_SCANCODE_END, ::Keyboard::Key::End },
        { SDL_SCANCODE_PAGEDOWN, ::Keyboard::Key::PageDown },
        { SDL_SCANCODE_RIGHT, ::Keyboard::Key::Right },
        { SDL_SCANCODE_LEFT, ::Keyboard::Key::Left },
        { SDL_SCANCODE_DOWN, ::Keyboard::Key::Down },
        { SDL_SCANCODE_UP, ::Keyboard::Key::Up },
        { SDL_SCANCODE_NUMLOCKCLEAR, ::Keyboard::Key::NumLockclear },
        { SDL_SCANCODE_KP_DIVIDE, ::Keyboard::Key::NumpadDivide },
        { SDL_SCANCODE_KP_MULTIPLY, ::Keyboard::Key::NumpadMultiply },
        { SDL_SCANCODE_KP_MINUS, ::Keyboard::Key::NumpadMinus },
        { SDL_SCANCODE_KP_PLUS, ::Keyboard::Key::NumpadPlus },
        { SDL_SCANCODE_KP_ENTER, ::Keyboard::Key::NumpadEnter },
        { SDL_SCANCODE_KP_1, ::Keyboard::Key::Numpad1 },
        { SDL_SCANCODE_KP_2, ::Keyboard::Key::Numpad2 },
        { SDL_SCANCODE_KP_3, ::Keyboard::Key::Numpad3 },
        { SDL_SCANCODE_KP_4, ::Keyboard::Key::Numpad4 },
        { SDL_SCANCODE_KP_5, ::Keyboard::Key::Numpad5 },
        { SDL_SCANCODE_KP_6, ::Keyboard::Key::Numpad6 },
        { SDL_SCANCODE_KP_7, ::Keyboard::Key::Numpad7 },
        { SDL_SCANCODE_KP_8, ::Keyboard::Key::Numpad8 },
        { SDL_SCANCODE_KP_9, ::Keyboard::Key::Numpad9 },
        { SDL_SCANCODE_KP_0, ::Keyboard::Key::Numpad0 },
        { SDL_SCANCODE_KP_PERIOD, ::Keyboard::Key::NumpadPeriod },
        { SDL_SCANCODE_NONUSBACKSLASH, ::Keyboard::Key::Nonusbackslash },
        { SDL_SCANCODE_APPLICATION, ::Keyboard::Key::Application },
        { SDL_SCANCODE_POWER, ::Keyboard::Key::Power },
        { SDL_SCANCODE_KP_EQUALS, ::Keyboard::Key::NumpadEquals },
        { SDL_SCANCODE_F13, ::Keyboard::Key::F13 },
        { SDL_SCANCODE_F14, ::Keyboard::Key::F14 },
        { SDL_SCANCODE_F15, ::Keyboard::Key::F15 },
        { SDL_SCANCODE_F16, ::Keyboard::Key::F16 },
        { SDL_SCANCODE_F17, ::Keyboard::Key::F17 },
        { SDL_SCANCODE_F18, ::Keyboard::Key::F18 },
        { SDL_SCANCODE_F19, ::Keyboard::Key::F19 },
        { SDL_SCANCODE_F20, ::Keyboard::Key::F20 },
        { SDL_SCANCODE_F21, ::Keyboard::Key::F21 },
        { SDL_SCANCODE_F22, ::Keyboard::Key::F22 },
        { SDL_SCANCODE_F23, ::Keyboard::Key::F23 },
        { SDL_SCANCODE_F24, ::Keyboard::Key::F24 },
        { SDL_SCANCODE_EXECUTE, ::Keyboard::Key::Execute },
        { SDL_SCANCODE_HELP, ::Keyboard::Key::Help },
        { SDL_SCANCODE_MENU, ::Keyboard::Key::Menu },
        { SDL_SCANCODE_SELECT, ::Keyboard::Key::Select },
        { SDL_SCANCODE_STOP, ::Keyboard::Key::Stop },
        { SDL_SCANCODE_AGAIN, ::Keyboard::Key::Again },
        { SDL_SCANCODE_UNDO, ::Keyboard::Key::Undo },
        { SDL_SCANCODE_CUT, ::Keyboard::Key::Cut },
        { SDL_SCANCODE_COPY, ::Keyboard::Key::Copy },
        { SDL_SCANCODE_PASTE, ::Keyboard::Key::Paste },
        { SDL_SCANCODE_FIND, ::Keyboard::Key::Find },
        { SDL_SCANCODE_MUTE, ::Keyboard::Key::Mute },
        { SDL_SCANCODE_VOLUMEUP, ::Keyboard::Key::VolumeUp },
        { SDL_SCANCODE_VOLUMEDOWN, ::Keyboard::Key::VolumeDown },
        //{LockingcapsLock, ,  ::Keyboard::KeySDL_SCANCODE_LOCKINGCAPSLOCK},
        //{LockingnumLock, ,  ::Keyboard::KeySDL_SCANCODE_LOCKINGNUMLOCK},
        //{LockingscrollLock, ,  ::Keyboard::KeySDL_SCANCODE_LOCKINGSCROLLLOCK},
        { SDL_SCANCODE_KP_COMMA, ::Keyboard::Key::NumpadComma },
        { SDL_SCANCODE_KP_EQUALSAS400, ::Keyboard::Key::NumpadEqualsas400 },
        { SDL_SCANCODE_INTERNATIONAL1, ::Keyboard::Key::International1 },
        { SDL_SCANCODE_INTERNATIONAL2, ::Keyboard::Key::International2 },
        { SDL_SCANCODE_INTERNATIONAL3, ::Keyboard::Key::International3 },
        { SDL_SCANCODE_INTERNATIONAL4, ::Keyboard::Key::International4 },
        { SDL_SCANCODE_INTERNATIONAL5, ::Keyboard::Key::International5 },
        { SDL_SCANCODE_INTERNATIONAL6, ::Keyboard::Key::International6 },
        { SDL_SCANCODE_INTERNATIONAL7, ::Keyboard::Key::International7 },
        { SDL_SCANCODE_INTERNATIONAL8, ::Keyboard::Key::International8 },
        { SDL_SCANCODE_INTERNATIONAL9, ::Keyboard::Key::International9 },
        { SDL_SCANCODE_LANG1, ::Keyboard::Key::Lang1 },
        { SDL_SCANCODE_LANG2, ::Keyboard::Key::Lang2 },
        { SDL_SCANCODE_LANG3, ::Keyboard::Key::Lang3 },
        { SDL_SCANCODE_LANG4, ::Keyboard::Key::Lang4 },
        { SDL_SCANCODE_LANG5, ::Keyboard::Key::Lang5 },
        { SDL_SCANCODE_LANG6, ::Keyboard::Key::Lang6 },
        { SDL_SCANCODE_LANG7, ::Keyboard::Key::Lang7 },
        { SDL_SCANCODE_LANG8, ::Keyboard::Key::Lang8 },
        { SDL_SCANCODE_LANG9, ::Keyboard::Key::Lang9 },
        { SDL_SCANCODE_ALTERASE, ::Keyboard::Key::AltErase },
        { SDL_SCANCODE_SYSREQ, ::Keyboard::Key::SysReq },
        { SDL_SCANCODE_CANCEL, ::Keyboard::Key::Cancel },
        { SDL_SCANCODE_CLEAR, ::Keyboard::Key::Clear },
        { SDL_SCANCODE_PRIOR, ::Keyboard::Key::Prior },
        { SDL_SCANCODE_RETURN2, ::Keyboard::Key::Return2 },
        { SDL_SCANCODE_SEPARATOR, ::Keyboard::Key::Separator },
        { SDL_SCANCODE_OUT, ::Keyboard::Key::Out },
        { SDL_SCANCODE_OPER, ::Keyboard::Key::Oper },
        { SDL_SCANCODE_CLEARAGAIN, ::Keyboard::Key::ClearAgain },
        { SDL_SCANCODE_CRSEL, ::Keyboard::Key::CrSel },
        { SDL_SCANCODE_EXSEL, ::Keyboard::Key::ExSel },
        { SDL_SCANCODE_KP_00, ::Keyboard::Key::Numpad00 },
        { SDL_SCANCODE_KP_000, ::Keyboard::Key::Numpad000 },
        { SDL_SCANCODE_THOUSANDSSEPARATOR, ::Keyboard::Key::ThousandsSeparator },
        { SDL_SCANCODE_DECIMALSEPARATOR, ::Keyboard::Key::DecimalSeparator },
        { SDL_SCANCODE_CURRENCYUNIT, ::Keyboard::Key::CurrencyUnit },
        { SDL_SCANCODE_CURRENCYSUBUNIT, ::Keyboard::Key::CurrencySubunit },
        { SDL_SCANCODE_KP_LEFTPAREN, ::Keyboard::Key::NumpadLeftParenthesis },
        { SDL_SCANCODE_KP_RIGHTPAREN, ::Keyboard::Key::NumpadRightParenthesis },
        { SDL_SCANCODE_KP_LEFTBRACE, ::Keyboard::Key::NumpadLeftBrace },
        { SDL_SCANCODE_KP_RIGHTBRACE, ::Keyboard::Key::NumpadRightBrace },
        { SDL_SCANCODE_KP_TAB, ::Keyboard::Key::NumpadTab },
        { SDL_SCANCODE_KP_BACKSPACE, ::Keyboard::Key::NumpadBackspace },
        { SDL_SCANCODE_KP_A, ::Keyboard::Key::NumpadA },
        { SDL_SCANCODE_KP_B, ::Keyboard::Key::NumpadB },
        { SDL_SCANCODE_KP_C, ::Keyboard::Key::NumpadC },
        { SDL_SCANCODE_KP_D, ::Keyboard::Key::NumpadD },
        { SDL_SCANCODE_KP_E, ::Keyboard::Key::NumpadE },
        { SDL_SCANCODE_KP_F, ::Keyboard::Key::NumpadF },
        { SDL_SCANCODE_KP_XOR, ::Keyboard::Key::NumpadXor },
        { SDL_SCANCODE_KP_POWER, ::Keyboard::Key::NumpadPower },
        { SDL_SCANCODE_KP_PERCENT, ::Keyboard::Key::NumpadPercent },
        { SDL_SCANCODE_KP_LESS, ::Keyboard::Key::NumpadLess },
        { SDL_SCANCODE_KP_GREATER, ::Keyboard::Key::NumpadGreater },
        { SDL_SCANCODE_KP_AMPERSAND, ::Keyboard::Key::NumpadAmpersand },
        { SDL_SCANCODE_KP_DBLAMPERSAND, ::Keyboard::Key::NumpadDblAmpersand },
        { SDL_SCANCODE_KP_VERTICALBAR, ::Keyboard::Key::NumpadVerticalBar },
        { SDL_SCANCODE_KP_DBLVERTICALBAR, ::Keyboard::Key::NumpadDblVerticalBar },
        { SDL_SCANCODE_KP_COLON, ::Keyboard::Key::NumpadColon },
        { SDL_SCANCODE_KP_HASH, ::Keyboard::Key::NumpadHash },
        { SDL_SCANCODE_KP_SPACE, ::Keyboard::Key::NumpadSpace },
        { SDL_SCANCODE_KP_AT, ::Keyboard::Key::NumpadAt },
        { SDL_SCANCODE_KP_EXCLAM, ::Keyboard::Key::NumpadExclam },
        { SDL_SCANCODE_KP_MEMSTORE, ::Keyboard::Key::NumpadMemStore },
        { SDL_SCANCODE_KP_MEMRECALL, ::Keyboard::Key::NumpadMemRecall },
        { SDL_SCANCODE_KP_MEMCLEAR, ::Keyboard::Key::NumpadMemClear },
        { SDL_SCANCODE_KP_MEMADD, ::Keyboard::Key::NumpadMemAdd },
        { SDL_SCANCODE_KP_MEMSUBTRACT, ::Keyboard::Key::NumpadMemSubtract },
        { SDL_SCANCODE_KP_MEMMULTIPLY, ::Keyboard::Key::NumpadMemMultiply },
        { SDL_SCANCODE_KP_MEMDIVIDE, ::Keyboard::Key::NumpadMemDivide },
        { SDL_SCANCODE_KP_PLUSMINUS, ::Keyboard::Key::NumpadPlusMinus },
        { SDL_SCANCODE_KP_CLEAR, ::Keyboard::Key::NumpadClear },
        { SDL_SCANCODE_KP_CLEARENTRY, ::Keyboard::Key::NumpadClearEntry },
        { SDL_SCANCODE_KP_BINARY, ::Keyboard::Key::NumpadBinary },
        { SDL_SCANCODE_KP_OCTAL, ::Keyboard::Key::NumpadOctal },
        { SDL_SCANCODE_KP_DECIMAL, ::Keyboard::Key::NumpadDecimal },
        { SDL_SCANCODE_KP_HEXADECIMAL, ::Keyboard::Key::NumpadHexadecimal },
        { SDL_SCANCODE_LCTRL, ::Keyboard::Key::LCtrl },
        { SDL_SCANCODE_LSHIFT, ::Keyboard::Key::LShift },
        { SDL_SCANCODE_LALT, ::Keyboard::Key::LAlt },
        { SDL_SCANCODE_LGUI, ::Keyboard::Key::LGui },
        { SDL_SCANCODE_RCTRL, ::Keyboard::Key::RCtrl },
        { SDL_SCANCODE_RSHIFT, ::Keyboard::Key::RShift },
        { SDL_SCANCODE_RALT, ::Keyboard::Key::RAlt },
        { SDL_SCANCODE_RGUI, ::Keyboard::Key::RGui },
        { SDL_SCANCODE_MODE, ::Keyboard::Key::Mode },
        { SDL_SCANCODE_AUDIONEXT, ::Keyboard::Key::AudioNext },
        { SDL_SCANCODE_AUDIOPREV, ::Keyboard::Key::AudioPrev },
        { SDL_SCANCODE_AUDIOSTOP, ::Keyboard::Key::AudioStop },
        { SDL_SCANCODE_AUDIOPLAY, ::Keyboard::Key::AudioPlay },
        { SDL_SCANCODE_AUDIOMUTE, ::Keyboard::Key::AudioMute },
        { SDL_SCANCODE_MEDIASELECT, ::Keyboard::Key::MediaSelect },
        { SDL_SCANCODE_WWW, ::Keyboard::Key::WWW },
        { SDL_SCANCODE_MAIL, ::Keyboard::Key::Mail },
        { SDL_SCANCODE_CALCULATOR, ::Keyboard::Key::Calculator },
        { SDL_SCANCODE_COMPUTER, ::Keyboard::Key::Computer },
        { SDL_SCANCODE_AC_SEARCH, ::Keyboard::Key::AcSearch },
        { SDL_SCANCODE_AC_HOME, ::Keyboard::Key::AcHome },
        { SDL_SCANCODE_AC_BACK, ::Keyboard::Key::AcBack },
        { SDL_SCANCODE_AC_FORWARD, ::Keyboard::Key::AcForward },
        { SDL_SCANCODE_AC_STOP, ::Keyboard::Key::AcStop },
        { SDL_SCANCODE_AC_REFRESH, ::Keyboard::Key::AcRefresh },
        { SDL_SCANCODE_AC_BOOKMARKS, ::Keyboard::Key::AcBookmarks },
        { SDL_SCANCODE_BRIGHTNESSDOWN, ::Keyboard::Key::BrightnessDown },
        { SDL_SCANCODE_BRIGHTNESSUP, ::Keyboard::Key::BrightnessUp },
        { SDL_SCANCODE_DISPLAYSWITCH, ::Keyboard::Key::DisplaySwitch },
        { SDL_SCANCODE_KBDILLUMTOGGLE, ::Keyboard::Key::KbdillumToggle },
        { SDL_SCANCODE_KBDILLUMDOWN, ::Keyboard::Key::KbdillumDown },
        { SDL_SCANCODE_KBDILLUMUP, ::Keyboard::Key::KbdillumUp },
        { SDL_SCANCODE_EJECT, ::Keyboard::Key::Eject },
        { SDL_SCANCODE_SLEEP, ::Keyboard::Key::Sleep },
        { SDL_SCANCODE_APP1, ::Keyboard::Key::App1 },
        { SDL_SCANCODE_APP2, ::Keyboard::Key::App2 },
        { SDL_SCANCODE_AUDIOREWIND, ::Keyboard::Key::AudioRewind },
        { SDL_SCANCODE_AUDIOFASTFORWARD, ::Keyboard::Key::AudioFastforward },
        { SDL_NUM_SCANCODES, ::Keyboard::Key::MaxValue }
    };
    const static auto s_reverseKeyLUT{ [&]() {
            std::unordered_map<::Keyboard::Key, SDL_Scancode> map;
            for (const auto& [key, value] : s_keyLUT)
                map[value] = key;
            return map;
        }()
    };
    auto GetKey(SDL_Scancode scanCode)
    {
        return s_keyLUT.at(scanCode);
    };
    Event::Keyboard CreateEventData(const SDL_KeyboardEvent& event)
    {
        return {
            Window::Get(event.windowID),
            Keyboard::GetKey(event.keysym.scancode),
            event.state == SDL_PRESSED,
            event.repeat != 0,
            (event.keysym.mod | KMOD_ALT) != 0,
            (event.keysym.mod | KMOD_CTRL) != 0,
            (event.keysym.mod | KMOD_SHIFT) != 0
        };
    }
    Event::TextEdit CreateEventData(const SDL_TextEditingEvent& event)
    {
        return {
            Window::Get(event.windowID),
            event.text,
            size_t(event.start),
            size_t(event.length)
        };
    }
    Event::TextInput CreateEventData(const SDL_TextInputEvent& event)
    {
        return {
            Window::Get(event.windowID),
            event.text
        };
    }
}
}

namespace Keyboard {
bool InputDevice::GetKeyState(Keyboard::Key key)
{
    return SDL_GetKeyboardState(nullptr)[size_t(SDL2::Keyboard::s_reverseKeyLUT.at(key))];
}

InputDevice::InputDevice() {
    EventsManager::On(Event::Type::KeyUp).ConnectMember(this, &InputDevice::_ProcessEvent);
    EventsManager::On(Event::Type::KeyDown).ConnectMember(this, &InputDevice::_ProcessEvent);
}
};
