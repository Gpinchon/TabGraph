/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:09:23
*/

#pragma once

#include "Component.hpp" // for Component
#include "Texture/PixelUtils.hpp"

#include <map>
#include <memory>

struct TextureSampler;

class Texture : public Component {
public:
    class Handle;
    class Impl;
    enum class Type {
        Unknown = -1,
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        Texture2DMultisample,
        Texture2DMultisampleArray,
        Texture3D,
        TextureBuffer,
        TextureCubemap,
        TextureCubemapArray,
        TextureRectangle,
        MaxValue
    };
    PROPERTY(std::shared_ptr<TextureSampler>, TextureSampler, nullptr);
    PROPERTY(int, MipMapNbr, 1);
    PROPERTY(bool, AutoMipMap, true);
    READONLYPROPERTY(Pixel::Description, PixelDescription, );
    READONLYPROPERTY(Texture::Type, Type, Texture::Type::Unknown);

public:
    Texture(const Texture&);
    Texture(Texture::Type target, Pixel::Description pixelDescription);
    Texture(Texture::Type target);
    ~Texture();

    void Load();
    void Unload();
    void GenerateMipmap();
    const Texture::Handle& GetHandle() const;

    bool GetLoaded() const;

    void SetPixelDescription(Pixel::Description pixelDescription);

    Impl& GetImpl();

protected:
    std::unique_ptr<Impl> _impl;
};
