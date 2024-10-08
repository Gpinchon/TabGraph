/*
* @Author: gpinchon
* @Date:   2021-04-30 14:23:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-29 22:09:18
*/
#pragma once

#include "Driver/OpenGL/ObjectHandle.hpp"
#include "Texture/Texture.hpp"

class Texture::Impl {
public:
    using Handle = OpenGL::ObjectHandle;
    Impl(const Impl& other);;
    Impl(Texture::Type);
    Impl(Texture::Type, const Pixel::Description&);
    ~Impl();
    void Bind() const;
    void Done() const;

    inline const Handle GetHandle() const
    {
        return _handle;
    }
    inline Texture::Type GetType() const
    {
        return _type;
    }
    inline std::shared_ptr<TextureSampler> GetTextureSampler() const
    {
        return _textureSampler;
    }
    inline uint8_t GetMipMapNbr() const
    {
        return _mipMapNbr;
    }
    inline bool GetAutoMipMap() const
    {
        return _autoMipMap;
    }
    inline Pixel::Description GetPixelDescription() const
    {
        return _pixelDescription;
    }

    inline void SetTextureSampler(std::shared_ptr<TextureSampler> textureSampler)
    {
        _textureSampler = textureSampler;
    }
    inline void SetMipMapNbr(const uint8_t mipMapNbr)
    {
        if (_mipMapNbr == mipMapNbr)
            return;
        _mipMapNbr = mipMapNbr;
        Unload();
    }
    inline void SetAutoMipMap(bool autoMipMap)
    {
        _autoMipMap = autoMipMap;
    }
    void SetPixelDescription(const Pixel::Description& pixelDesc);
    inline void SetLoaded(bool value)
    {
        _loaded = value;
    }
    inline bool GetLoaded() const
    {
        return _loaded;
    }

    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual void GenerateMipmap() = 0;

    //OpenGL specific settings
    enum class FormatCompatibilityType {
        Size,
        Class,
        None,
        MaxValue
    };
    enum class DepthStencilTextureMode {
        DepthComponent,
        StencilIndex,
        MaxValue
    };

protected:
    Handle _handle { 0 };
    bool _loaded { false };
    Texture::Type _type { Texture::Type::Unknown };
    std::shared_ptr<TextureSampler> _textureSampler { nullptr };
    uint8_t _mipMapNbr { 1 };
    bool _autoMipMap { true };
    Pixel::Description _pixelDescription;
};

namespace OpenGL {
unsigned GetEnum(::Texture::Type);
namespace Texture {
    ::Texture::Impl::Handle Generate();
    void Delete(::Texture::Impl::Handle);
};
};
