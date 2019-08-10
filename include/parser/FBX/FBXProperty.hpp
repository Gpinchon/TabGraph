/*
* @Author: gpinchon
* @Date:   2019-08-10 11:23:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-10 12:40:40
*/

#pragma once

#include <stdint.h>
#include <string>
#include <variant>
#include <zconf.h>

namespace FBX {
typedef std::variant<Byte*, char*, float*, double*, int32_t*, int64_t*>
    ArrayData;

struct Array {
    uint32_t length;
    uint32_t encoding;
    uint32_t compressedLength;
    ArrayData data;
    template <typename T>
    operator T() const { return std::get<T>(data); }
    operator std::string() const { return std::get<char*>(data); };
};

typedef std::variant<Byte, float, double, int16_t, int32_t, int64_t, Array>
    PropertyData;

struct Property {
    unsigned char typeCode;
    PropertyData data;
    virtual void Print() const;
    template <typename T>
    operator T() const { return std::get<T>(data); }
    operator std::string() const { return std::get<Array>(data); }
};
}
