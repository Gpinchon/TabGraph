/*
* @Author: gpinchon
* @Date:   2021-04-30 14:23:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-29 22:09:18
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Texture/Texture.hpp>
#include <Texture/PixelUtils.hpp>
#include <Driver/OpenGL/ObjectHandle.hpp>
#include <Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture::Impl {
    READONLYPROPERTY(TabGraph::Pixel::Description, PixelDescription, );
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
    inline std::shared_ptr<Textures::Sampler> GetTextureSampler() const
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

    inline void SetTextureSampler(std::shared_ptr<Textures::Sampler> textureSampler)
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
    std::shared_ptr<Textures::Sampler> _textureSampler { nullptr };
    uint8_t _mipMapNbr { 1 };
    bool _autoMipMap { true };
};
}

namespace OpenGL {
    unsigned GetEnum(TabGraph::Textures::Texture::Type);
    namespace Texture {
        TabGraph::Textures::Texture::Impl::Handle Generate();
        void Delete(TabGraph::Textures::Texture::Impl::Handle);
    };
};
