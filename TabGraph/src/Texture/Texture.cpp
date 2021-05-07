/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-30 14:22:57
*/

#include "Texture/Texture.hpp"
#include "Texture/TextureSampler.hpp"
#include "Debug.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Texture/Texture.hpp"
//#endif

#include <algorithm>
#include <cstring>
#include <stdint.h>
#include <utility>
#include <array>

Texture::Texture(Texture::Type type)
    : Component()
{
    SetTextureSampler(std::make_shared<TextureSampler>());
    _SetType(type);
}

Texture::Texture(const Texture& other)
    : Component(other)
    , _Type(other._Type)
    , _TextureSampler(other._TextureSampler)
    , _PixelDescription(other._PixelDescription)
{
}

Texture::Texture(Texture::Type type, Pixel::Description pixelDesc)
    : Texture(type)
{
    _SetPixelDescription(pixelDesc);
}

Texture::~Texture()
{
    Unload();
}

void Texture::Load()
{
    _impl->Load();
}

void Texture::Unload()
{
    _impl->Unload();
}

void Texture::GenerateMipmap()
{
    _impl->GenerateMipmap();
}

void Texture::SetPixelDescription(Pixel::Description pixelDesc)
{
    _SetPixelDescription(pixelDesc);
    _impl->Unload();
}

Texture::Impl& Texture::GetImpl()
{
    return *_impl;
}

const Texture::Handle& Texture::GetHandle() const
{
    return _impl->GetHandle();
}

bool Texture::GetLoaded() const
{
    return _impl->GetLoaded();
}
