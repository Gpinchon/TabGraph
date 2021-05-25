/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 17:31:26
*/

#include <Event/InputDevice/Keyboard.hpp>
#include <Event/Keyboard.hpp>

#if MEDIALIBRARY == SDL2
#include <Driver/SDL2/Keyboard.hpp>
#endif //MEDIALIBRARY == SDL2

namespace Keyboard {
InputDevice& Get() {
    static std::unique_ptr<InputDevice> s_instance(new InputDevice);
    return *s_instance;
}

bool GetKeyState(Key key)
{
    return Get().GetKeyState(key);
}

Signal<const Event::Keyboard&>& OnKey(Key key)
{
    return Get().onKey.at(size_t(key));
}

Signal<const Event::Keyboard&>& OnKeyUp(Key key)
{
    return Get().onKeyUp.at(size_t(key));
}

Signal<const Event::Keyboard&>& OnKeyDown(Key key)
{
    return Get().onKeyDown.at(size_t(key));
}
};
