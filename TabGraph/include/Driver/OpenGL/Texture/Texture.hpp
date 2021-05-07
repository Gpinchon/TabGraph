/*
* @Author: gpinchon
* @Date:   2021-04-30 14:23:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-30 19:16:39
*/
#pragma once

#include "Driver/OpenGL/ObjectHandle.hpp"
#include "Texture/Texture.hpp"

class Texture::Handle : public OpenGL::ObjectHandle {
public:
    Handle(unsigned v) : OpenGL::ObjectHandle(v) {};
};

class Texture::Impl {
public:
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
    Impl(const Impl&) = delete;
    Impl(Texture&);
    const Texture::Handle GetHandle() const;
    void Bind() const;
    void Done() const;
    virtual void Load() = 0;
    virtual void Unload() = 0;
    virtual void GenerateMipmap() = 0;

    void SetLoaded(bool value);
    bool GetLoaded() const;

    //OpenGL specific settings
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
    Texture::Handle _handle{ 0 };
    Texture& _texture;
    bool _loaded{ false };
};

namespace OpenGL {
namespace Texture {
unsigned GetGLEnum(::Texture::Type);
::Texture::Handle Generate();
void Delete(::Texture::Handle);
};
};
