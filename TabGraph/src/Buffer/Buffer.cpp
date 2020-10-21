/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-10-21 14:25:51
*/
#include "Buffer/Buffer.hpp"
#include "Debug.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <stdio.h>
#include <wchar.h>

Buffer::Buffer(size_t byteLength, GLenum usage)
    : Component("")
    , _byteLength(byteLength) //_rawData(byteLength, std::byte(0))
    , _usage(usage)
{
}

Buffer::~Buffer()
{
    //Unload();
    _UnloadCPU();
    _UnloadGPU();
}

std::shared_ptr<Buffer> Buffer::Create(size_t byteLength, GLenum usage)
{
    return std::shared_ptr<Buffer>(new Buffer(byteLength, usage));
}

void Buffer::_UpdateGPU(float)
{
    if (!_rawData.empty())
        std::memcpy(Map(BufferAccess::Write), _rawData.data(), ByteLength());
    Unmap();
}

#include <cassert>

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

auto GetUnmapRangeFunction()
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

void Buffer::Allocate()
{
    debugLog(Name());
    if (Glid() > 0)
        glDeleteBuffers(1, &_glid);
    if (glCreateBuffers == nullptr) //We are not using OGL 4.5
    {
        glGenBuffers(1, &_glid);
        glNamedBufferDataEXT(
            Glid(),
            ByteLength(),
            nullptr,
            Usage());
    } else {
        glCreateBuffers(1, &_glid);
        glNamedBufferData(
            Glid(),
            ByteLength(),
            nullptr,
            Usage());
    }
}

bool Buffer::Mapped() const
{
    return _mapped;
}

void* Buffer::MappingPointer() const
{
    return _mappingPointer;
}

void* Buffer::Map(GLenum access)
{
    if (Glid() == 0)
        Allocate();
    if (Mapped())
        throw std::runtime_error("Buffer" + Name() + " : is already mapped.");
    auto ptr(GetMapFunction()(Glid(), access));
    _mapped = true;
    _mappingPointer = ptr;
    return ptr;
}

void* Buffer::MapRange(size_t offset, size_t length, GLbitfield access)
{
    if (Glid() == 0)
        Allocate();
    if (Mapped())
        throw std::runtime_error("Buffer" + Name() + " : is already mapped.");
    auto ptr(GetMapRangeFunction()(Glid(), offset, length, access));
    _mapped = true;
    _mappingPointer = ptr;
    return ptr;
}

void Buffer::Unmap()
{
    if (!Mapped()) {
        debugLog(Name() + " IS NOT MAPPED");
        return;
    }
    GetUnmapRangeFunction()(Glid());
    _mapped = false;
    _mappingPointer = nullptr;
}

void Buffer::Load()
{
    _LoadCPU();
    _LoadGPU();
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

void Buffer::_LoadCPU()
{
    if (LoadedCPU())
        return;
    debugLog(Name());
    _rawData.resize(ByteLength(), std::byte(0));
    if (Uri() != "") {
        auto data(ParseData(Uri().string()));
        if (data.empty()) {
            debugLog(Uri());
            std::ifstream is(Uri(), std::ios::binary);
            if (!is.read((char*)_rawData.data(), ByteLength()))
                throw std::runtime_error(Uri().string() + ": " + std::strerror(errno));
            if (is.gcount() != ByteLength())
                throw std::runtime_error(Name() + " : Incomplete buffer read");
        } else
            _rawData = data;
    }
    SetLoadedCPU(true);
}

void Buffer::_LoadGPU()
{
    if (LoadedGPU())
        return;
    debugLog(Name());
    Allocate();
    if (Uri() != "") {
        auto data(ParseData(Uri().string()));
        if (data.empty()) {
            debugLog(Uri());
            debugLog(ByteLength());
            std::ifstream is(Uri(), std::ios::binary);
            if (!is.read((char*)Map(BufferAccess::Write), ByteLength())) {
                Unmap();
                throw std::runtime_error(Uri().string() + ": " + std::strerror(errno));
            }
            Unmap();
            if (is.gcount() != ByteLength())
                throw std::runtime_error(Name() + " : Incomplete buffer read");
        } else {
            std::memcpy(Map(BufferAccess::Write), data.data(), ByteLength());
            Unmap();
        }
    } else if (!_rawData.empty()) {
        _UpdateGPU(0.f);
    }
    SetLoadedGPU(true);
}

//void Buffer::Unload()
//{
//    _UnloadCPU();
//    _UnloadGPU();
//}

void Buffer::_UnloadCPU()
{
    _rawData.resize(0);
    _rawData.shrink_to_fit();
    SetLoadedCPU(false);
}

void Buffer::_UnloadGPU()
{
    Unmap();
    glDeleteBuffers(1, &_glid);
    _glid = 0;
    SetLoadedGPU(false);
}

std::vector<std::byte>& Buffer::RawData()
{
    _LoadCPU();
    return _rawData;
}

std::filesystem::path Buffer::Uri() const
{
    return _uri;
}

void Buffer::SetUri(std::string uri)
{
    _uri = uri;
}

size_t Buffer::ByteLength() const
{
    return _byteLength;
    //return _rawData.size();
}

void Buffer::SetByteLength(size_t byteLength)
{
    _byteLength = byteLength;
    //_rawData.resize(byteLength);
}

GLenum Buffer::Usage() const
{
    return _usage;
}

void Buffer::SetUsage(GLenum usage)
{
    _usage = usage;
}

GLuint Buffer::Glid() const
{
    return _glid;
}
