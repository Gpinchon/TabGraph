/*
* @Author: gpinchon
* @Date:   2021-05-01 20:13:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-01 22:32:59
*/
#pragma once

namespace OpenGL {
class ObjectHandle {
public:
    ObjectHandle(unsigned v)
        : _v(v)
    {
    }
    unsigned* operator &()
    {
        return &_v;
    }
    operator unsigned& ()
    {
        return _v;
    }
    operator const unsigned& () const
    {
        return _v;
    }
    bool operator!=(const ObjectHandle& other)
    {
        return _v != other._v;
    }
    auto operator=(unsigned v)
    {
        return _v = v;
    }

private:
    unsigned _v{ 0 };
};
};
