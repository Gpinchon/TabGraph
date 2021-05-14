/*
* @Author: gpinchon
* @Date:   2021-04-30 14:23:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-30 19:16:39
*/
#pragma once

#include "Driver/OpenGL/ObjectHandle.hpp"
#include "Texture/Texture.hpp"

class Texture::Impl {
public:
    using Handle = OpenGL::ObjectHandle;
    Impl(const Impl&) = delete;
    Impl(Texture&);
    const Handle GetHandle() const;
    void Bind() const;
    void Done() const;
    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual void GenerateMipmap() = 0;

    void SetLoaded(bool value);
    bool GetLoaded() const;

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
    void SetDepthStencilTextureMode(DepthStencilTextureMode value);
    DepthStencilTextureMode GetDepthStencilTextureMode() const;

    //OpenGL specific getters
    FormatCompatibilityType GetFormatCompatibilityType() const;
    bool GetImmutableFormat() const;
    int GetImmutableLevels() const;
    Type GetTarget() const;
    int GetViewMinLayer() const;
    int GetViewMinLevel() const;
    int GetViewNumLayers() const;
    int GetViewNumLevels() const;
    int GetMinLevel() const;
    void SetMinLevel(int value);
    int GetMaxLevel() const;
    void SetMaxLevel(int value);

protected:
    Handle _handle{ 0 };
    Texture& _texture;
    bool _loaded{ false };
};

namespace OpenGL {
unsigned GetEnum(::Texture::Type);
namespace Texture {
::Texture::Impl::Handle Generate();
void Delete(::Texture::Impl::Handle);
};
};
