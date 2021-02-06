/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:02
*/

#pragma once

#include "Component.hpp" // for Component
#include "Texture/PixelUtils.hpp"

#include <GL/glew.h>
#include <map>



class Texture : public Component {
public:
    using Handle = GLuint;
    enum class Type : GLenum {
        Unknown = -1,
        Texture1D = GL_TEXTURE_1D,
        Texture2D = GL_TEXTURE_2D,
        Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
        Texture3D = GL_TEXTURE_2D_ARRAY,
        Texture3DMultisample = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
        TextureBuffer = GL_TEXTURE_BUFFER,
        TextureCubemap = GL_TEXTURE_CUBE_MAP
    };
    READONLYPROPERTY(Pixel::Description, PixelDescription, );
    READONLYPROPERTY(Texture::Type, Type, Texture::Type::Unknown);
    READONLYPROPERTY(Texture::Handle, Handle, 0);
    READONLYPROPERTY(bool, Loaded, false);
    PROPERTY(uint8_t, Multisample, 0);

public:
    Texture(Texture::Type target, Pixel::Description pixelDescription);
    Texture(Texture::Type target);
    ~Texture();
    template <typename T>
    void SetParameter(GLenum p, T v);
    void RestoreParameters();
    virtual void Load() = 0;
    virtual void Unload();
    virtual void GenerateMipmap();

    void SetPixelDescription(Pixel::Description pixelDescription);

    static Handle Create(Texture::Type);

private:
    std::map<GLenum, float> _parametersf;
    std::map<GLenum, int32_t> _parametersi;
    std::map<GLenum, float*> _parametersfv;
    std::map<GLenum, int32_t*> _parametersiv;
    std::map<GLenum, uint32_t*> _parametersuiv;
};

template <>
inline void Texture::SetParameter(GLenum p, int32_t v)
{
    if (GetLoaded()) {
        if (glTextureParameteri == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameteri((GLenum)GetType(), p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameteri(GetHandle(), p, v);
        }
    }
    _parametersi[p] = v;
}

template <>
inline void Texture::SetParameter(GLenum p, uint32_t v)
{
    SetParameter(p, int32_t(v));
}


template <>
inline void Texture::SetParameter(GLenum p, float v)
{
    if (GetLoaded()) {
        if (glTextureParameterf == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameterf((GLenum)GetType(), p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameterf(GetHandle(), p, v);
        }
    }
    _parametersf[p] = v;
}

template <>
inline void Texture::SetParameter(GLenum p, float* v)
{
    if (GetLoaded()) {
        if (glTextureParameterfv == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameterfv((GLenum)GetType(), p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameterfv(GetHandle(), p, v);
        }
    }
    _parametersfv[p] = v;
}

template <>
inline void Texture::SetParameter(GLenum p, int* v)
{
    if (GetLoaded()) {
        if (glTextureParameteriv == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameteriv((GLenum)GetType(), p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameteriv(GetHandle(), p, v);
        }
    }
    _parametersiv[p] = v;
}

template <>
inline void Texture::SetParameter(GLenum p, uint32_t* v)
{
    if (GetLoaded()) {
        if (glTextureParameterIuiv == nullptr) {
            glBindTexture((GLenum)GetType(), GetHandle());
            glTexParameterIuiv((GLenum)GetType(), p, v);
            glBindTexture((GLenum)GetType(), 0);
        }
        else {
            glTextureParameterIuiv(GetHandle(), p, v);
        }
    }
    _parametersuiv[p] = v;
}

#include <exception>

template<typename T>
inline void Texture::SetParameter(GLenum p, T v)
{
    throw std::runtime_error("Unknown parameter type");
}
