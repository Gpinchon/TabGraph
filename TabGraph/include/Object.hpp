/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:41
*/

#pragma once

#include "Event/Signal.hpp"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <memory> // for enable_shared_from_this, shared_ptr
#include <string> // for string
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector> // for vector

/*
** Never allocate on the stack, always of the heap !!!
** ALWAYS STORE IN A SHARED_PTR !!!
*/

/** Use this to declare a new property */
#define PROPERTY(type, var, ...)   \
public:                            \
    Signal<type> var##Changed;     \
    type Get##var() const          \
    {                              \
        return _##var;             \
    }                              \
    void Set##var(const type& val) \
    {                              \
        if (val != _##var) {       \
            _##var = val;          \
            var##Changed(val);     \
        }                          \
    }                              \
                                   \
private:                           \
    type _##var { __VA_ARGS__ };

#define READONLYPROPERTY(type, var, ...) \
public:                             \
    Signal<type> var##Changed;      \
    type Get##var() const           \
    {                               \
        return _##var;              \
    }                               \
                                    \
protected:                          \
    void _Set##var(const type& val) \
    {                               \
        if (val != _##var) {        \
            _##var = val;           \
            var##Changed(val);      \
        }                           \
    }                               \
private:                            \
    type _##var { __VA_ARGS__ };

#define PRIVATEPROPERTY(type, var, ...) \
private:                                \
    Signal<type> var##Changed;          \
    type _Get##var() const              \
    {                                   \
        return _##var;                  \
    }                                   \
    void _Set##var(const type& val)     \
    {                                   \
        bool changed = val != _##var;   \
        _##var = val;                   \
        if (changed)                    \
            var##Changed(val);          \
    }                                   \
    type _##var { __VA_ARGS__ };

class Object : public std::enable_shared_from_this<Object>, public Trackable {
    PROPERTY(int64_t, Id, reinterpret_cast<int64_t>(this));
    PROPERTY(std::string, Name, "");

public:
    Object();
    Object(const std::string& name);
    virtual ~Object();
};
