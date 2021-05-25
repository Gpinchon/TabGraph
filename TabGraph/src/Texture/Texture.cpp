/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-11 13:41:54
*/

#include "Texture/Texture.hpp"
#include "Debug.hpp"
#include "Texture/TextureSampler.hpp"
//#if RENDERINGAPI == OpenGL
#include "Driver/OpenGL/Texture/Texture.hpp"
//#endif

#include <algorithm>
#include <array>
#include <cstring>
#include <stdint.h>
#include <utility>

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

bool Texture::GetLoaded() const
{
    return _impl->GetLoaded();
}
