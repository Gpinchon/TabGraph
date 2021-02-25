#pragma once

#include "Event/Signal.hpp"

/** Use this to declare a new property */
#define PROPERTY(type, var, ...)   \
public:                            \
    Signal<type> var##Changed;     \
    type Get##var() const          \
    {                              \
        return _##var;             \
    }                              \
    void Set##var(type val)        \
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
    void _Set##var(type val)        \
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
    void _Set##var(type val)            \
    {                                   \
        bool changed = val != _##var;   \
        _##var = val;                   \
        if (changed)                    \
            var##Changed(val);          \
    }                                   \
    type _##var { __VA_ARGS__ };