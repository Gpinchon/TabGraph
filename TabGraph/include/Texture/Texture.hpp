/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:02
*/

#pragma once

#include "Component.hpp" // for Component
#include "Texture/PixelUtils.hpp"

#include <GL/glew.h>
#include <map>

class Texture : public Component {
public:
    using Handle = GLuint;
    enum class Parameter : GLenum {
        LodBias = GL_TEXTURE_LOD_BIAS,
        BaseLevel = GL_TEXTURE_BASE_LEVEL,
        MaxLevel = GL_TEXTURE_MAX_LEVEL,
        WrapS = GL_TEXTURE_WRAP_S,
        WrapT = GL_TEXTURE_WRAP_T,
        WrapR = GL_TEXTURE_WRAP_R,
        MinFilter = GL_TEXTURE_MIN_FILTER,
        MagFilter = GL_TEXTURE_MAG_FILTER,
        SwizzleR = GL_TEXTURE_SWIZZLE_R,
        SwizzleG = GL_TEXTURE_SWIZZLE_G,
        SwizzleB = GL_TEXTURE_SWIZZLE_B,
        SwizzleA = GL_TEXTURE_SWIZZLE_A,
        SwizzleRGBA = GL_TEXTURE_SWIZZLE_RGBA,
        BorderColor = GL_TEXTURE_BORDER_COLOR,
        CompareMode = GL_TEXTURE_COMPARE_MODE,
        CompareFunc = GL_TEXTURE_COMPARE_FUNC
    };
    enum class Wrap : GLenum {
        Repeat = GL_REPEAT,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        MirroredClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
    };
    enum class Filter : GLenum {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST
    };
    enum class Swizzle : GLenum {
        Zero = GL_ZERO,
        One = GL_ONE,
        Red = GL_RED,
        Green = GL_GREEN,
        Blue = GL_BLUE,
        Alpha = GL_ALPHA
    };
    enum class CompareMode : GLenum {
        None = GL_NONE,
        CompareRefToTexture = GL_COMPARE_REF_TO_TEXTURE
    };
    enum class CompareFunc : GLenum {
        LessEqual = GL_LEQUAL,
        GreaterEqual = GL_GEQUAL,
        Less = GL_LESS,
        Greater = GL_GREATER,
        Equal = GL_EQUAL,
        NotEqual = GL_NOTEQUAL,
        Always = GL_ALWAYS,
        Never = GL_NEVER
    };
    enum class Type : GLenum {
        Unknown = -1,
        Texture1D = GL_TEXTURE_1D,
        Texture2D = GL_TEXTURE_2D,
        Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
        Texture3D = GL_TEXTURE_2D_ARRAY,
        Texture3DMultisample = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
        TextureBuffer = GL_TEXTURE_BUFFER,
        TextureCubemap = GL_TEXTURE_CUBE_MAP
    };

    READONLYPROPERTY(Pixel::Description, PixelDescription, );
    READONLYPROPERTY(Texture::Type, Type, Texture::Type::Unknown);
    READONLYPROPERTY(Texture::Handle, Handle, 0);
    READONLYPROPERTY(bool, Loaded, false);
    PROPERTY(uint8_t, Multisample, 0);

public:
    Texture(Texture::Type target, Pixel::Description pixelDescription);
    Texture(Texture::Type target);
    ~Texture();
    template <Parameter parameter, typename... Params>
    void SetParameter(Params...);
    void RestoreParameters();
    virtual void Load() = 0;
    virtual void Unload();
    virtual void GenerateMipmap();

    void SetPixelDescription(Pixel::Description pixelDescription);

    static Handle Create(Texture::Type);

private:
    void _SetParameterf(Texture::Parameter, const float);
    void _SetParameteri(Texture::Parameter, const int32_t);
    void _SetParameterfv(Texture::Parameter, const float*);
    void _SetParameteriv(Texture::Parameter, const int32_t*);
    std::map<Texture::Parameter, float> _parametersf;
    std::map<Texture::Parameter, int32_t> _parametersi;
    std::map<Texture::Parameter, std::vector<float>> _parametersfv;
    std::map<Texture::Parameter, std::vector<int32_t>> _parametersiv;
};

template<Texture::Parameter parameter, typename... Params>
inline void Texture::SetParameter(Params...)
{
    Params::Unknown_parameter_type;
}

template<>
inline void Texture::SetParameter<Texture::Parameter::LodBias>(float value)
{
    _parametersf[Texture::Parameter::LodBias] = value;
    _SetParameterf(Texture::Parameter::LodBias, value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::BaseLevel>(int value)
{
    _parametersi[Texture::Parameter::BaseLevel] = value;
    _SetParameteri(Texture::Parameter::BaseLevel, value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::MaxLevel>(int value)
{
    _parametersi[Texture::Parameter::MaxLevel] = value;
    _SetParameteri(Texture::Parameter::MaxLevel, value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::WrapS>(Texture::Wrap value)
{
    _parametersi[Texture::Parameter::WrapS] = (int)value;
    _SetParameteri(Texture::Parameter::WrapS, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::WrapT>(Texture::Wrap value)
{
    _parametersi[Texture::Parameter::WrapT] = (int)value;
    _SetParameteri(Texture::Parameter::WrapT, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::WrapR>(Texture::Wrap value)
{
    _parametersi[Texture::Parameter::WrapR] = (int)value;
    _SetParameteri(Texture::Parameter::WrapR, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::MinFilter>(Texture::Filter value)
{
    _parametersi[Texture::Parameter::MinFilter] = (int)value;
    _SetParameteri(Texture::Parameter::MinFilter, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::MagFilter>(Texture::Filter value)
{
    _parametersi[Texture::Parameter::MagFilter] = (int)value;
    _SetParameteri(Texture::Parameter::MagFilter, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::SwizzleR>(Texture::Swizzle value)
{
    _parametersi[Texture::Parameter::SwizzleR] = (int)value;
    _SetParameteri(Texture::Parameter::SwizzleR, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::SwizzleG>(Texture::Swizzle value)
{
    _parametersi[Texture::Parameter::SwizzleG] = (int)value;
    _SetParameteri(Texture::Parameter::SwizzleG, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::SwizzleB>(Texture::Swizzle value)
{
    _parametersi[Texture::Parameter::SwizzleB] = (int)value;
    _SetParameteri(Texture::Parameter::SwizzleB, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::SwizzleA>(Texture::Swizzle value)
{
    _parametersi[Texture::Parameter::SwizzleA] = (int)value;
    _SetParameteri(Texture::Parameter::SwizzleA, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::SwizzleRGBA>(Texture::Swizzle swizzleR, Texture::Swizzle swizzleG, Texture::Swizzle swizzleB, Texture::Swizzle swizzleA)
{
    auto value = { (int)swizzleR, (int)swizzleG, (int)swizzleB, (int)swizzleA };
    _parametersiv[Texture::Parameter::SwizzleRGBA] = value;
    _SetParameteriv(Texture::Parameter::SwizzleRGBA, value.begin());
}

template<>
inline void Texture::SetParameter<Texture::Parameter::BorderColor>(int r, int g, int b, int a)
{
    auto value = { r, g, b, a };
    _parametersiv[Texture::Parameter::BorderColor] = value;
    _SetParameteriv(Texture::Parameter::BorderColor, value.begin());
}

template<>
inline void Texture::SetParameter<Texture::Parameter::BorderColor>(float r, float g, float b, float a)
{
    auto value = { r, g, b, a };
    _parametersfv[Texture::Parameter::BorderColor] = value;
    _SetParameterfv(Texture::Parameter::BorderColor, value.begin());
}

template<>
inline void Texture::SetParameter<Texture::Parameter::CompareMode>(Texture::CompareMode value)
{
    _parametersi[Texture::Parameter::CompareMode] = (int)value;
    _SetParameteri(Texture::Parameter::CompareMode, (int)value);
}

template<>
inline void Texture::SetParameter<Texture::Parameter::CompareFunc>(Texture::CompareFunc value)
{
    _parametersi[Texture::Parameter::CompareFunc] = (int)value;
    _SetParameteri(Texture::Parameter::CompareFunc, (int)value);
}
