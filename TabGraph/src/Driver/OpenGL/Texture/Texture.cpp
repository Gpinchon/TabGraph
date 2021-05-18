/*
* @Author: gpinchon
* @Date:   2021-04-30 13:13:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-07 15:35:59
*/

#include "Driver/OpenGL/Texture/Texture.hpp"

#include <GL/glew.h>

unsigned OpenGL::GetEnum(::Texture::Type type)
{
    switch (type) {
    case ::Texture::Type::Texture1D:
        return GL_TEXTURE_1D;
    case ::Texture::Type::Texture1DArray:
        return GL_TEXTURE_1D_ARRAY;
    case ::Texture::Type::Texture2D:
        return GL_TEXTURE_2D;
    case ::Texture::Type::Texture2DArray:
        return GL_TEXTURE_2D_ARRAY;
    case ::Texture::Type::Texture2DMultisample:
        return GL_TEXTURE_2D_MULTISAMPLE;
    case ::Texture::Type::Texture2DMultisampleArray:
        return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    case ::Texture::Type::Texture3D:
        return GL_TEXTURE_3D;
    case ::Texture::Type::TextureBuffer:
        return GL_TEXTURE_BUFFER;
    case ::Texture::Type::TextureCubemap:
        return GL_TEXTURE_CUBE_MAP;
    case ::Texture::Type::TextureCubemapArray:
        return GL_TEXTURE_CUBE_MAP_ARRAY;
    case ::Texture::Type::TextureRectangle:
        return GL_TEXTURE_RECTANGLE;
    default:
        throw std::runtime_error("Unknown Texture::Type");
    }
}

::Texture::Impl::Handle OpenGL::Texture::Generate()
{
    unsigned handle;
    glGenTextures(1, &handle);
    return handle;
}

void OpenGL::Texture::Delete(::Texture::Impl::Handle handle)
{
    glDeleteTextures(1, &handle);
}

Texture::Impl::Impl(Texture& texture)
    : _texture(texture)
{
}

void Texture::Impl::SetLoaded(bool value)
{
    _loaded = value;
}

bool Texture::Impl::GetLoaded() const
{
    return _loaded;
}

const Texture::Impl::Handle Texture::Impl::GetHandle() const
{
    return _handle;
}

void Texture::Impl::Bind() const
{
    glBindTexture(OpenGL::GetEnum(_texture.GetType()), GetHandle());
}

void Texture::Impl::Done() const
{
    glBindTexture(OpenGL::GetEnum(_texture.GetType()), 0);
}

int Texture::Impl::GetMinLevel() const
{
    int value;
    Bind();
    glGetTexParameteriv(OpenGL::GetEnum(_texture.GetType()), GL_TEXTURE_BASE_LEVEL, &value);
    Done();
    return value;
}

void Texture::Impl::SetMinLevel(int value)
{
    Bind();
    glTexParameteri(OpenGL::GetEnum(_texture.GetType()), GL_TEXTURE_BASE_LEVEL, value);
    Done();
}

int Texture::Impl::GetMaxLevel() const
{
    return 0;
}

void Texture::Impl::SetMaxLevel(int value)
{
    Bind();
    glTexParameteri(OpenGL::GetEnum(_texture.GetType()), GL_TEXTURE_MAX_LEVEL, value);
    Done();
}
