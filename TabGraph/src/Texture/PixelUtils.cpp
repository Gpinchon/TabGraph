/*
* @Author: gpinchon
* @Date:   2021-01-12 18:26:34
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-21 17:07:05
*/
#include "Texture\PixelUtils.hpp"

inline Pixel::SizedFormat GetRSizedformat(Pixel::Type type, bool normalizedType)
{
    assert(type != Pixel::Type::Unknown);
    switch (type) {
    case Pixel::Type::Uint8:
        return normalizedType ? Pixel::SizedFormat::Uint8_NormalizedR : Pixel::SizedFormat::Uint8_R;
    case Pixel::Type::Int8:
        return normalizedType ? Pixel::SizedFormat::Int8_NormalizedR : Pixel::SizedFormat::Int8_R;
    case Pixel::Type::Uint16:
        return normalizedType ? Pixel::SizedFormat::Uint16_NormalizedR : Pixel::SizedFormat::Uint16_R;
    case Pixel::Type::Int16:
        return normalizedType ? Pixel::SizedFormat::Int16_NormalizedR : Pixel::SizedFormat::Int16_R;
    case Pixel::Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return Pixel::SizedFormat::Uint32_R;
    case Pixel::Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return Pixel::SizedFormat::Int32_R;
    case Pixel::Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return Pixel::SizedFormat::Float16_R;
    case Pixel::Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return Pixel::SizedFormat::Float32_R;
    }
}

inline Pixel::SizedFormat GetRGSizedformat(Pixel::Type type, bool normalizedType)
{
    assert(type != Pixel::Type::Unknown);
    switch (type) {
    case Pixel::Type::Uint8:
        return normalizedType ? Pixel::SizedFormat::Uint8_NormalizedRG : Pixel::SizedFormat::Uint8_RG;
    case Pixel::Type::Int8:
        return normalizedType ? Pixel::SizedFormat::Int8_NormalizedRG : Pixel::SizedFormat::Int8_RG;
    case Pixel::Type::Uint16:
        return normalizedType ? Pixel::SizedFormat::Uint16_NormalizedRG : Pixel::SizedFormat::Uint16_RG;
    case Pixel::Type::Int16:
        return normalizedType ? Pixel::SizedFormat::Int16_NormalizedRG : Pixel::SizedFormat::Int16_RG;
    case Pixel::Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return Pixel::SizedFormat::Uint32_RG;
    case Pixel::Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return Pixel::SizedFormat::Int32_RG;
    case Pixel::Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return Pixel::SizedFormat::Float16_RG;
    case Pixel::Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return Pixel::SizedFormat::Float32_RG;
    }
}

inline Pixel::SizedFormat GetRGBSizedformat(Pixel::Type type, bool normalizedType)
{
    assert(type != Pixel::Type::Unknown);
    switch (type) {
    case Pixel::Type::Uint8:
        return normalizedType ? Pixel::SizedFormat::Uint8_NormalizedRGB : Pixel::SizedFormat::Uint8_RGB;
    case Pixel::Type::Int8:
        return normalizedType ? Pixel::SizedFormat::Int8_NormalizedRGB : Pixel::SizedFormat::Int8_RGB;
    case Pixel::Type::Uint16:
        return normalizedType ? Pixel::SizedFormat::Uint16_NormalizedRGB : Pixel::SizedFormat::Uint16_RGB;
    case Pixel::Type::Int16:
        return normalizedType ? Pixel::SizedFormat::Int16_NormalizedRGB : Pixel::SizedFormat::Int16_RGB;
    case Pixel::Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return Pixel::SizedFormat::Uint32_RGB;
    case Pixel::Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return Pixel::SizedFormat::Int32_RGB;
    case Pixel::Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return Pixel::SizedFormat::Float16_RGB;
    case Pixel::Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return Pixel::SizedFormat::Float32_RGB;
    }
}

inline Pixel::SizedFormat GetRGBASizedFormat(Pixel::Type type, bool normalizedType)
{
    assert(type != Pixel::Type::Unknown);
    switch (type) {
    case Pixel::Type::Uint8:
        return normalizedType ? Pixel::SizedFormat::Uint8_NormalizedRGBA : Pixel::SizedFormat::Uint8_RGBA;
    case Pixel::Type::Int8:
        return normalizedType ? Pixel::SizedFormat::Int8_NormalizedRGBA : Pixel::SizedFormat::Int8_RGBA;
    case Pixel::Type::Uint16:
        return normalizedType ? Pixel::SizedFormat::Uint16_NormalizedRGBA : Pixel::SizedFormat::Uint16_RGBA;
    case Pixel::Type::Int16:
        return normalizedType ? Pixel::SizedFormat::Int16_NormalizedRGBA : Pixel::SizedFormat::Int16_RGBA;
    case Pixel::Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return Pixel::SizedFormat::Uint32_RGBA;
    case Pixel::Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return Pixel::SizedFormat::Int32_RGBA;
    case Pixel::Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return Pixel::SizedFormat::Float16_RGBA;
    case Pixel::Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return Pixel::SizedFormat::Float32_RGBA;
    }
}

Pixel::SizedFormat GetDepthSizedformat(Pixel::Type type, bool normalizedType)
{
    assert(type != Pixel::Type::Unknown);
    switch (type) {
    case Pixel::Type::Uint8:
        assert(true && "Depth texture cannot be of type Uint8");
        break;
    case Pixel::Type::Int8:
        assert(true && "Depth texture cannot be of type Int8");
        break;
    case Pixel::Type::Uint16:
        assert(true && "Depth texture cannot be of type Uint16");
        break;
    case Pixel::Type::Int16:
        assert(true && "Depth texture cannot be of type Int16");
        break;
    case Pixel::Type::Uint32:
        return Pixel::SizedFormat::Depth32;
        break;
    case Pixel::Type::Int32:
        assert(true && "Depth texture cannot be of type Int32");
        break;
    case Pixel::Type::Float16:
        return Pixel::SizedFormat::Depth16;
    case Pixel::Type::Float32:
        return Pixel::SizedFormat::Depth32F;
    }
}

Pixel::SizedFormat GetDepthStencilSizedFormat(Pixel::Type type, bool normalizedType)
{
    assert(type != Pixel::Type::Unknown);
    switch (type)
    {
    case Pixel::Type::Uint8:
        assert(true && "Depth texture cannot be of type Uint8");
        break;
    case Pixel::Type::Int8:
        assert(true && "Depth texture cannot be of type Int8");
        break;
    case Pixel::Type::Uint16:
        assert(true && "Depth texture cannot be of type Uint16");
        break;
    case Pixel::Type::Int16:
        assert(true && "Depth texture cannot be of type Int16");
        break;
    case Pixel::Type::Uint32:
        return Pixel::SizedFormat::Depth24_Stencil8;
        break;
    case Pixel::Type::Int32:
        assert(true && "Depth texture cannot be of type Int32");
        break;
    case Pixel::Type::Float16:
        assert(true && "Depth texture cannot be of type Float16");
        break;
    case Pixel::Type::Float32:
        return Pixel::SizedFormat::Depth32F_Stencil8;
        break;
    }
    
}

Pixel::SizedFormat GetStencilSizedFormat(Pixel::Type type, bool normalizedType)
{
    assert(type != Pixel::Type::Unknown);
    switch (type)
    {
    case Pixel::Type::Uint8:
        return Pixel::SizedFormat::Stencil8;
        break;
    case Pixel::Type::Int8:
        assert(true && "Stencil texture cannot be of type Int8");
        break;
    case Pixel::Type::Uint16:
        assert(true && "Stencil texture cannot be of type Uint16");
        break;
    case Pixel::Type::Int16:
        assert(true && "Stencil texture cannot be of type Int16");
        break;
    case Pixel::Type::Uint32:
        assert(true && "Stencil texture cannot be of type Uint32");
        break;
    case Pixel::Type::Int32:
        assert(true && "Stencil texture cannot be of type Int32");
        break;
    case Pixel::Type::Float16:
        assert(true && "Stencil texture cannot be of type Float16");
        break;
    case Pixel::Type::Float32:
        assert(true && "Stencil texture cannot be of type Float32");
        break;
    }
}

#include <glm/glm.hpp>

glm::vec4 Pixel::LinearToSRGB(glm::vec4 color)
{
    const glm::vec3 linearRGB{ color.r, color.g, color.b };
    glm::bvec3 cutoff = lessThan(linearRGB, glm::vec3(0.0031308));
    glm::vec3 higher = glm::vec3(1.055) * pow(linearRGB, glm::vec3(1.0 / 2.4)) - glm::vec3(0.055);
    glm::vec3 lower = linearRGB * glm::vec3(12.92);

    return glm::vec4(mix(higher, lower, cutoff), color.a);
}

Pixel::SizedFormat Pixel::GetSizedFormat(UnsizedFormat unsizedFormat, Type type, bool normalized)
{
    assert(unsizedFormat != Pixel::UnsizedFormat::Unknown);
    switch (unsizedFormat) {
    case UnsizedFormat::R:
        return GetRSizedformat(type, normalized);
    case UnsizedFormat::RG:
        return GetRGSizedformat(type, normalized);
    case UnsizedFormat::RGB:
        return GetRGBSizedformat(type, normalized);
    case UnsizedFormat::RGBA:
        return GetRGBASizedFormat(type, normalized);
    case UnsizedFormat::Depth:
        return GetDepthSizedformat(type, normalized);
    case UnsizedFormat::Depth_Stencil:
        return GetDepthStencilSizedFormat(type, normalized);
    case UnsizedFormat::Stencil:
        return GetStencilSizedFormat(type, normalized);
    }
}

uint8_t Pixel::GetUnsizedFormatComponentsNbr(UnsizedFormat format)
{
    assert(format != Pixel::UnsizedFormat::Unknown);
    switch (format) {
    case UnsizedFormat::R:
    case UnsizedFormat::R_Integer:
    case UnsizedFormat::Depth:
    case UnsizedFormat::Stencil:
        return 1;
    case UnsizedFormat::RG:
    case UnsizedFormat::RG_Integer:
    case UnsizedFormat::Depth_Stencil:
        return 2;
    case UnsizedFormat::RGB:
    case UnsizedFormat::RGB_Integer:
        return 3;
    case UnsizedFormat::RGBA:
    case UnsizedFormat::RGBA_Integer:
        return 4;
    }
}

uint8_t Pixel::GetTypeSize(Type type)
{
    assert(type != Pixel::Type::Unknown);
    switch (type) {
    case Type::Uint8:
    case Type::Int8:
        return 1;
    case Type::Uint16:
    case Type::Int16:
    case Type::Float16:
        return 2;
    case Type::Uint32:
    case Type::Int32:
    case Type::Float32:
        return 4;
    }
}

uint8_t Pixel::GetOctetsPerPixels(UnsizedFormat format, Type Type)
{
    return GetUnsizedFormatComponentsNbr(format) * GetTypeSize(Type);
}

Pixel::Description::Description(UnsizedFormat format, Type type, bool normalized)
    : Pixel::Description(Pixel::GetSizedFormat(format, type, normalized))
{
    /*_sizedFormat(Pixel::GetSizedFormat(format, type, normalized))
    , _unsizedFormat(format)
    , _type(type)
    , _typeSize(Pixel::GetTypeSize(type))
    , _components(GetUnsizedFormatComponentsNbr(format))
    , _size(_components * _typeSize)
    , _normalized(normalized)*/
}

Pixel::Description::Description(SizedFormat format)
{
    assert(format != Pixel::SizedFormat::Unknown);
    switch (format) {
    case SizedFormat::Uint8_NormalizedR:
        _unsizedFormat = UnsizedFormat::R;
        _type = Type::Uint8;
        _normalized = true;
        break;
    case SizedFormat::Uint8_NormalizedRG:
        _unsizedFormat = UnsizedFormat::RG;
        _type = Type::Uint8;
        _normalized = true;
        break;
    case SizedFormat::Uint8_NormalizedRGB:
        _unsizedFormat = UnsizedFormat::RGB;
        _type = Type::Uint8;
        _normalized = true;
        break;
    case SizedFormat::Uint8_NormalizedRGBA:
        _unsizedFormat = UnsizedFormat::RGBA;
        _type = Type::Uint8;
        _normalized = true;
        _hasAlpha = true;
        break;
    case SizedFormat::Int8_NormalizedR:
        _unsizedFormat = UnsizedFormat::R;
        _type = Type::Int8;
        _normalized = true;
        break;
    case SizedFormat::Int8_NormalizedRG:
        _unsizedFormat = UnsizedFormat::RG;
        _type = Type::Int8;
        _normalized = true;
        break;
    case SizedFormat::Int8_NormalizedRGB:
        _unsizedFormat = UnsizedFormat::RGB;
        _type = Type::Int8;
        _normalized = true;
        break;
    case SizedFormat::Int8_NormalizedRGBA:
        _unsizedFormat = UnsizedFormat::RGBA;
        _type = Type::Int8;
        _normalized = true;
        _hasAlpha = true;
        break;
    case SizedFormat::Uint8_R:
        _unsizedFormat = UnsizedFormat::R_Integer;
        _type = Type::Uint8;
        break;
    case SizedFormat::Uint8_RG:
        _unsizedFormat = UnsizedFormat::RG_Integer;
        _type = Type::Uint8;
        break;
    case SizedFormat::Uint8_RGB:
        _unsizedFormat = UnsizedFormat::RGB_Integer;
        _type = Type::Uint8;
        break;
    case SizedFormat::Uint8_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA_Integer;
        _type = Type::Uint8;
        _hasAlpha = true;
        break;
    case SizedFormat::Int8_R:
        _unsizedFormat = UnsizedFormat::R_Integer;
        _type = Type::Int8;
        break;
    case SizedFormat::Int8_RG:
        _unsizedFormat = UnsizedFormat::RG_Integer;
        _type = Type::Int8;
        break;
    case SizedFormat::Int8_RGB:
        _unsizedFormat = UnsizedFormat::RGB_Integer;
        _type = Type::Int8;
        break;
    case SizedFormat::Int8_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA_Integer;
        _type = Type::Int8;
        _hasAlpha = true;
        break;
    case SizedFormat::Uint16_NormalizedR:
        _unsizedFormat = UnsizedFormat::R;
        _type = Type::Uint16;
        _normalized = true;
        break;
    case SizedFormat::Uint16_NormalizedRG:
        _unsizedFormat = UnsizedFormat::RG;
        _type = Type::Uint16;
        _normalized = true;
        break;
    case SizedFormat::Uint16_NormalizedRGB:
        _unsizedFormat = UnsizedFormat::RGB;
        _type = Type::Uint16;
        _normalized = true;
        break;
    case SizedFormat::Uint16_NormalizedRGBA:
        _unsizedFormat = UnsizedFormat::RGBA;
        _type = Type::Uint16;
        _normalized = true;
        _hasAlpha = true;
        break;
    case SizedFormat::Int16_NormalizedR:
        _unsizedFormat = UnsizedFormat::R;
        _type = Type::Int16;
        _normalized = true;
        break;
    case SizedFormat::Int16_NormalizedRG:
        _unsizedFormat = UnsizedFormat::RG;
        _type = Type::Int16;
        _normalized = true;
        break;
    case SizedFormat::Int16_NormalizedRGB:
        _unsizedFormat = UnsizedFormat::RGB;
        _type = Type::Int16;
        _normalized = true;
        break;
    case SizedFormat::Int16_NormalizedRGBA:
        _unsizedFormat = UnsizedFormat::RGBA;
        _type = Type::Int16;
        _normalized = true;
        _hasAlpha = true;
        break;
    case SizedFormat::Uint16_R:
        _unsizedFormat = UnsizedFormat::R_Integer;
        _type = Type::Uint16;
        break;
    case SizedFormat::Uint16_RG:
        _unsizedFormat = UnsizedFormat::RG_Integer;
        _type = Type::Uint16;
        break;
    case SizedFormat::Uint16_RGB:
        _unsizedFormat = UnsizedFormat::RGB_Integer;
        _type = Type::Uint16;
        break;
    case SizedFormat::Uint16_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA_Integer;
        _type = Type::Uint16;
        _hasAlpha = true;
        break;
    case SizedFormat::Int16_R:
        _unsizedFormat = UnsizedFormat::R_Integer;
        _type = Type::Int16;
        break;
    case SizedFormat::Int16_RG:
        _unsizedFormat = UnsizedFormat::RG_Integer;
        _type = Type::Int16;
        break;
    case SizedFormat::Int16_RGB:
        _unsizedFormat = UnsizedFormat::RGB_Integer;
        _type = Type::Int16;
        break;
    case SizedFormat::Int16_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA_Integer;
        _type = Type::Int16;
        _hasAlpha = true;
        break;
    case SizedFormat::Uint32_R:
        _unsizedFormat = UnsizedFormat::R_Integer;
        _type = Type::Uint32;
        break;
    case SizedFormat::Uint32_RG:
        _unsizedFormat = UnsizedFormat::RG_Integer;
        _type = Type::Uint32;
        break;
    case SizedFormat::Uint32_RGB:
        _unsizedFormat = UnsizedFormat::RGB_Integer;
        _type = Type::Uint32;
        break;
    case SizedFormat::Uint32_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA_Integer;
        _type = Type::Uint32;
        _hasAlpha = true;
        break;
    case SizedFormat::Int32_R:
        _unsizedFormat = UnsizedFormat::R_Integer;
        _type = Type::Int32;
        break;
    case SizedFormat::Int32_RG:
        _unsizedFormat = UnsizedFormat::RG_Integer;
        _type = Type::Int32;
        break;
    case SizedFormat::Int32_RGB:
        _unsizedFormat = UnsizedFormat::RGB_Integer;
        _type = Type::Int32;
        break;
    case SizedFormat::Int32_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA_Integer;
        _type = Type::Int32;
        _hasAlpha = true;
        break;
    case SizedFormat::Float16_R:
        _unsizedFormat = UnsizedFormat::R;
        _type = Type::Float16;
        break;
    case SizedFormat::Float16_RG:
        _unsizedFormat = UnsizedFormat::RG;
        _type = Type::Float16;
        break;
    case SizedFormat::Float16_RGB:
        _unsizedFormat = UnsizedFormat::RGB;
        _type = Type::Float16;
        break;
    case SizedFormat::Float16_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA;
        _type = Type::Float16;
        _hasAlpha = true;
        break;
    case SizedFormat::Float32_R:
        _unsizedFormat = UnsizedFormat::R;
        _type = Type::Float32;
        break;
    case SizedFormat::Float32_RG:
        _unsizedFormat = UnsizedFormat::RG;
        _type = Type::Float32;
        break;
    case SizedFormat::Float32_RGB:
        _unsizedFormat = UnsizedFormat::RGB;
        _type = Type::Float32;
        break;
    case SizedFormat::Float32_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA;
        _type = Type::Float32;
        _hasAlpha = true;
        break;
    case SizedFormat::Depth16:
        _unsizedFormat;
        _type = Type::Float16;
        break;
    case SizedFormat::Depth24:
        _unsizedFormat = UnsizedFormat::Depth;
        _type = Type::Float32;
        break;
    case SizedFormat::Depth32:
        _unsizedFormat = UnsizedFormat::Depth;
        _type = Type::Float32;
        break;
    case SizedFormat::Depth32F:
        _unsizedFormat = UnsizedFormat::Depth;
        _type = Type::Float32;
        break;
    case SizedFormat::Depth24_Stencil8:
        _unsizedFormat = UnsizedFormat::Depth;
        _type = Type::Uint32;
        break;
    case SizedFormat::Depth32F_Stencil8:
        _unsizedFormat = UnsizedFormat::Depth_Stencil;
        _type = Type::Uint32;
        break;
    case SizedFormat::Stencil8:
        _unsizedFormat = UnsizedFormat::Stencil;
        _type = Type::Uint8;
        break;
    }
    _sizedFormat = format;
    _typeSize = Pixel::GetTypeSize(_type);
    _components = Pixel::GetUnsizedFormatComponentsNbr(_unsizedFormat);
    _size = _components * _typeSize;
}

Pixel::SizedFormat Pixel::Description::GetSizedFormat() const
{
    return _sizedFormat;
}

Pixel::UnsizedFormat Pixel::Description::GetUnsizedFormat() const
{
    return _unsizedFormat;
}

Pixel::Type Pixel::Description::GetType() const
{
    return _type;
}

uint8_t Pixel::Description::GetTypeSize() const
{
    return _typeSize;
}

uint8_t Pixel::Description::GetComponents() const
{
    return _components;
}

uint8_t Pixel::Description::GetSize() const
{
    return _size;
}

bool Pixel::Description::GetNormalized() const
{
    return _normalized;
}

#include <glm/detail/type_half.hpp>

float GetNormalizedColorComponent(Pixel::Type type, std::byte* bytes)
{
    assert(type != Pixel::Type::Unknown);
    assert(type != Pixel::Type::Uint32 && "Uint32 textures cannot be normalized");
    assert(type != Pixel::Type::Int32 && "Int32 textures cannot be normalized");
    assert(type != Pixel::Type::Float16 && "Float16 textures cannot be normalized");
    assert(type != Pixel::Type::Float32 && "Float32 textures cannot be normalized");
    switch (type) {
    case Pixel::Type::Uint8:
        return *reinterpret_cast<uint8_t*>(bytes) / float(UINT8_MAX);
    case Pixel::Type::Int8:
        return *reinterpret_cast<int8_t*>(bytes) / float(INT8_MAX);
    case Pixel::Type::Uint16:
        return *reinterpret_cast<uint16_t*>(bytes) / float(UINT16_MAX);
    case Pixel::Type::Int16:
        return *reinterpret_cast<int16_t*>(bytes) / float(INT16_MAX);
    }
}

float GetColorComponent(Pixel::Type type, std::byte* bytes)
{
    assert(type != Pixel::Type::Unknown);
    switch (type) {
    case Pixel::Type::Uint8:
        return *reinterpret_cast<uint8_t*>(bytes);
    case Pixel::Type::Int8:
        return *reinterpret_cast<int8_t*>(bytes);
    case Pixel::Type::Uint16:
        return *reinterpret_cast<uint16_t*>(bytes);
    case Pixel::Type::Int16:
        return *reinterpret_cast<int16_t*>(bytes);
    case Pixel::Type::Uint32:
        return *reinterpret_cast<uint32_t*>(bytes);
    case Pixel::Type::Int32:
        return *reinterpret_cast<int32_t*>(bytes);
    case Pixel::Type::Float16:
        return glm::detail::toFloat32(*reinterpret_cast<glm::detail::hdata*>(bytes));
    case Pixel::Type::Float32:
        return *reinterpret_cast<float*>(bytes);
    }
}

Pixel::Color Pixel::Description::GetColorFromBytes(std::byte* bytes) const
{
    auto getComponent = GetNormalized() ? &GetNormalizedColorComponent : &GetColorComponent;
    Color color { 0, 0, 0, 1 };
    switch (GetComponents()) {
    case 1:
        color[0] = getComponent(GetType(), &bytes[GetTypeSize() * 0]);
        break;
    case 2:
        color[0] = getComponent(GetType(), &bytes[GetTypeSize() * 0]);
        color[1] = getComponent(GetType(), &bytes[GetTypeSize() * 1]);
        break;
    case 3:
        color[0] = getComponent(GetType(), &bytes[GetTypeSize() * 0]);
        color[1] = getComponent(GetType(), &bytes[GetTypeSize() * 1]);
        color[2] = getComponent(GetType(), &bytes[GetTypeSize() * 2]);
        break;
    case 4:
        color[0] = getComponent(GetType(), &bytes[GetTypeSize() * 0]);
        color[1] = getComponent(GetType(), &bytes[GetTypeSize() * 1]);
        color[2] = getComponent(GetType(), &bytes[GetTypeSize() * 2]);
        color[3] = getComponent(GetType(), &bytes[GetTypeSize() * 3]);
        break;
    }
    return color;
}

#include <algorithm>

void SetComponentNormalized(Pixel::Type type, std::byte* bytes, float component)
{
    assert(type != Pixel::Type::Unknown);
    assert(type != Pixel::Type::Uint32 && "Uint32 textures cannot be normalized");
    assert(type != Pixel::Type::Int32 && "Int32 textures cannot be normalized");
    assert(type != Pixel::Type::Float16 && "Float16 textures cannot be normalized");
    assert(type != Pixel::Type::Float32 && "Float32 textures cannot be normalized");
    switch (type) {
    case Pixel::Type::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = std::clamp(component, 0.f, 1.f) * float(UINT8_MAX);
        break;
    case Pixel::Type::Int8:
        *reinterpret_cast<int8_t*>(bytes) = std::clamp(component, -1.f, 1.f) * float(INT8_MAX);
        break;
    case Pixel::Type::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = std::clamp(component, 0.f, 1.f) * float(UINT16_MAX);
        break;
    case Pixel::Type::Int16:
        *reinterpret_cast<int16_t*>(bytes) = std::clamp(component, -1.f, 1.f) * float(INT16_MAX);
        break;
    }
}

void SetComponent(Pixel::Type type, std::byte* bytes, float component)
{
    assert(type != Pixel::Type::Unknown);
    switch (type) {
    case Pixel::Type::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = component;
        break;
    case Pixel::Type::Int8:
        *reinterpret_cast<int8_t*>(bytes) = component;
        break;
    case Pixel::Type::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = component;
        break;
    case Pixel::Type::Int16:
        *reinterpret_cast<int16_t*>(bytes) = component;
        break;
    case Pixel::Type::Uint32:
        *reinterpret_cast<uint32_t*>(bytes) = component;
        break;
    case Pixel::Type::Int32:
        *reinterpret_cast<int32_t*>(bytes) = component;
        break;
    case Pixel::Type::Float16:
        *reinterpret_cast<glm::detail::hdata*>(bytes) = glm::detail::toFloat16(component);
        break;
    case Pixel::Type::Float32:
        *reinterpret_cast<float*>(bytes) = component;
        break;
    }
}

void Pixel::Description::SetColorToBytes(std::byte* bytes, const Color& color) const
{
    auto setComponent = GetNormalized() ? SetComponentNormalized : SetComponent;
    switch (GetComponents()) {
    case 1:
        setComponent(GetType(), &bytes[GetTypeSize() * 0], color[0]);
        break;
    case 2:
        setComponent(GetType(), &bytes[GetTypeSize() * 0], color[0]);
        setComponent(GetType(), &bytes[GetTypeSize() * 1], color[1]);
        break;
    case 3:
        setComponent(GetType(), &bytes[GetTypeSize() * 0], color[0]);
        setComponent(GetType(), &bytes[GetTypeSize() * 1], color[1]);
        setComponent(GetType(), &bytes[GetTypeSize() * 2], color[2]);
        break;
    case 4:
        setComponent(GetType(), &bytes[GetTypeSize() * 0], color[0]);
        setComponent(GetType(), &bytes[GetTypeSize() * 1], color[1]);
        setComponent(GetType(), &bytes[GetTypeSize() * 2], color[2]);
        setComponent(GetType(), &bytes[GetTypeSize() * 3], color[3]);
        break;
    }
}
