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
public:
    Texture(const Texture&);
    Texture();
    ~Texture();
    void Load();
    void Unload();
    void GenerateMipmap();

    Texture::Type GetType() const;
    Pixel::Description GetPixelDescription() const;
    std::shared_ptr<TextureSampler> GetTextureSampler() const;
    bool GetLoaded() const;
    bool GetAutoMipMap() const;
    uint8_t GetMipMapNbr() const;

    void SetAutoMipMap(bool autoMipmap);
    void SetTextureSampler(std::shared_ptr<TextureSampler> textureSampler);
    void SetMipMapNbr(uint8_t mipNbr);
    

    Impl& GetImpl();

protected:
    std::unique_ptr<Impl> _impl;
};
