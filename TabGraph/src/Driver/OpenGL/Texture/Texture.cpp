/*
* @Author: gpinchon
* @Date:   2021-04-30 13:13:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-07 15:35:59
*/

#include "Driver/OpenGL/Texture/Texture.hpp"
#include "Texture/TextureSampler.hpp"

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

Texture::Impl::Impl(const Impl& other)
    : _type(other._type)
    , _textureSampler(other._textureSampler)
    , _mipMapNbr(other._mipMapNbr)
    , _autoMipMap(other._autoMipMap)
    , _pixelDescription(other._pixelDescription)
{
}

Texture::Impl::Impl(Texture::Type type)
    : _type(type)
    , _textureSampler(std::make_shared<TextureSampler>())
{
}

Texture::Impl::Impl(Texture::Type type, const Pixel::Description& description)
    : _type(type)
    , _pixelDescription(description)
    , _textureSampler(std::make_shared<TextureSampler>())
{
}

Texture::Impl::~Impl()
{
    OpenGL::Texture::Delete(_handle);
}

void Texture::Impl::Bind() const
{
    glBindTexture(OpenGL::GetEnum(GetType()), GetHandle());
}

void Texture::Impl::Done() const
{
    glBindTexture(OpenGL::GetEnum(GetType()), 0);
}

void Texture::Impl::SetPixelDescription(const Pixel::Description& pixelDesc)
{
    if (pixelDesc == GetPixelDescription())
        return;
    _pixelDescription = pixelDesc;
    Unload();
}
