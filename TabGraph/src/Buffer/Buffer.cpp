/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-10-21 14:25:51
*/
#include "Buffer/Buffer.hpp"
#include "Debug.hpp"
#include "Render.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <stdio.h>
#include <wchar.h>

size_t bufferNbr = 0;

Buffer::Buffer(size_t byteLength)
    : Component("Buffer_" + std::to_string(bufferNbr))
    , _byteLength { byteLength }
    , _data { byteLength }
{
    bufferNbr++;
}

Buffer::~Buffer()
{
    Unload();
}

auto GetMapFunction()
{
    static auto f = glMapNamedBuffer ? glMapNamedBuffer : glMapNamedBufferEXT;
    assert(f);
    return f;
}

auto GetMapRangeFunction()
{
    static auto f = glMapNamedBufferRange ? glMapNamedBufferRange : glMapNamedBufferRangeEXT;
    assert(f);
    return f;
}

auto GetUnmapFunction()
{
    static auto f = glUnmapNamedBuffer ? glUnmapNamedBuffer : glUnmapNamedBufferEXT;
    assert(f);
    return f;
}

auto GetCreateFunction()
{
    static auto f = glCreateBuffers ? glCreateBuffers : glGenBuffers;
    assert(f);
    return f;
}

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string)
{
    int in_len = static_cast<int>(encoded_string.size());
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz"
                                     "0123456789+/";

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }

    return ret;
}

auto ParseData(const std::string& uri)
{
    std::vector<std::byte> ret;
    std::string header("data:application/octet-stream;base64,");
    if (uri.find(header) == 0) {
        auto data(base64_decode(uri.substr(header.size())));
        std::transform(data.begin(), data.end(), ret.begin(),
            [](char c) { return std::byte(c); });
    }
    return ret;
}

#include <fstream>

void Buffer::Load()
{
    if (GetLoaded())
        return;
    debugLog(GetName());
    _data.resize(GetByteLength());
    if (GetUri() != "") {
        auto data(ParseData(GetUri().string()));
        if (data.empty()) {
            debugLog(GetUri());
            debugLog(GetByteLength());
            std::ifstream is(GetUri(), std::ios::binary);
            if (!is.read((char*)Get(0), GetByteLength())) {
                throw std::runtime_error(GetUri().string() + ": " + std::strerror(errno));
            }
            if (is.gcount() != GetByteLength())
                throw std::runtime_error(GetName() + " : Incomplete buffer read");
        } else {
            Set(data.data(), 0, data.size());
        }
    }
    SetLoaded(true);
}

void Buffer::Unload()
{
    if (!GetLoaded())
        return;
    _data.resize(0);
    SetLoaded(false);
}

size_t Buffer::GetByteLength() const
{
    return _byteLength;
}

void Buffer::SetByteLength(size_t byteLength)
{
    _data.resize(byteLength);
    _byteLength = byteLength;
    SetLoaded(false);
}
