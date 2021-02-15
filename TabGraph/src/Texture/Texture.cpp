/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:42:10
*/

#include "Texture/Texture.hpp"
//#include "Buffer/Buffer.hpp"
//#include "Config.hpp" // for Config
#include "Debug.hpp" // for glCheckError, debugLog
#include <algorithm> // for min
#include <cstring> // for memcpy
#include <stdint.h> // for int16_t, uint64_t
#include <utility> // for pair

Texture::Texture(Texture::Type target, Pixel::Description pixelDesc)
    : Component()
{
    _SetType(target);
    SetPixelDescription(pixelDesc);
}

Texture::Texture(Texture::Type target)
{
    _SetType(target);
}

Texture::~Texture()
{
    Unload();
}

void Texture::RestoreParameters()
{
    for (const auto parameterf : _parametersf)
        _SetParameterf(parameterf.first, parameterf.second);
    for (const auto parameteri : _parametersi)
        _SetParameteri(parameteri.first, parameteri.second);
    for (const auto parameterfv : _parametersfv)
        _SetParameterfv(parameterfv.first, parameterfv.second.data());
    for (const auto parameteriv : _parametersiv)
        _SetParameteriv(parameteriv.first, parameteriv.second.data());
}

void Texture::Unload()
{
    GLuint id = GetHandle();
    _SetHandle(0);
    glDeleteTextures(1, &id);
    _SetLoaded(false);
}

void Texture::GenerateMipmap()
{
    glGenerateTextureMipmap(GetHandle());
    SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::LinearMipmapLinear);
}

void Texture::SetPixelDescription(Pixel::Description pixelDesc)
{
    _SetPixelDescription(pixelDesc);
    _SetLoaded(false);
}

Texture::Handle Texture::Create(Texture::Type type)
{
    GLuint glid;
    glCreateTextures((GLenum)type, 1, &glid);
    return glid;
}

void Texture::_SetParameterf(Texture::Parameter p, const float v)
{
    if (GetLoaded()) {
        if (glTextureParameterf == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameterf((GLenum)GetType(), (GLenum)p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameterf(GetHandle(), (GLenum)p, v);
        }
    }
}

void Texture::_SetParameteri(Texture::Parameter p, const int32_t v)
{
    if (GetLoaded()) {
        if (glTextureParameteri == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameteri((GLenum)GetType(), (GLenum)p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameteri(GetHandle(), (GLenum)p, v);
        }
    }
}

void Texture::_SetParameterfv(Texture::Parameter p, const float* v)
{
    if (GetLoaded()) {
        if (glTextureParameterfv == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameterfv((GLenum)GetType(), (GLenum)p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameterfv(GetHandle(), (GLenum)p, v);
        }
    }
}

void Texture::_SetParameteriv(Texture::Parameter p, const int32_t* v)
{
    if (GetLoaded()) {
        if (glTextureParameteriv == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameteriv((GLenum)GetType(), (GLenum)p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameteriv(GetHandle(), (GLenum)p, v);
        }
    }
}
