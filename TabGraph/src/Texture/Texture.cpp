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

Texture::Texture()
    : Component()
{
}

Texture::Texture(const Texture& other)
    : Component(other)
{
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

Texture::Type Texture::GetType() const
{
    return _impl->GetType();
}

Texture::Impl& Texture::GetImpl()
{
    return *_impl;
}

void Texture::SetAutoMipMap(bool autoMipmap)
{
    return _impl->SetAutoMipMap(autoMipmap);
}

void Texture::SetTextureSampler(std::shared_ptr<TextureSampler> textureSampler)
{
    return _impl->SetTextureSampler(textureSampler);
}

bool Texture::GetLoaded() const
{
    return _impl->GetLoaded();
}

Pixel::Description Texture::GetPixelDescription() const
{
    return _impl->GetPixelDescription();
}

std::shared_ptr<TextureSampler> Texture::GetTextureSampler() const
{
    return _impl->GetTextureSampler();
}

void Texture::SetMipMapNbr(uint8_t mipNbr)
{
    return _impl->SetMipMapNbr(mipNbr);
}

bool Texture::GetAutoMipMap() const
{
    return _impl->GetAutoMipMap();
}

uint8_t Texture::GetMipMapNbr() const
{
    return _impl->GetMipMapNbr();
}
