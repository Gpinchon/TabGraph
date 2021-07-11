/*
* @Author: gpinchon
* @Date:   2021-01-12 18:26:34
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-29 16:56:46
*/
#include <Texture/PixelUtils.hpp>

#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/detail/type_half.hpp>

namespace TabGraph::Pixel {
inline SizedFormat GetRSizedformat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return normalizedType ? SizedFormat::Uint8_NormalizedR : SizedFormat::Uint8_R;
    case Type::Int8:
        return normalizedType ? SizedFormat::Int8_NormalizedR : SizedFormat::Int8_R;
    case Type::Uint16:
        return normalizedType ? SizedFormat::Uint16_NormalizedR : SizedFormat::Uint16_R;
    case Type::Int16:
        return normalizedType ? SizedFormat::Int16_NormalizedR : SizedFormat::Int16_R;
    case Type::Uint32:
        assert(!normalizedType && "uint2 pixel type cannot be normalized");
        return SizedFormat::Uint32_R;
    case Type::Int32:
        assert(!normalizedType && "int32 pixel type cannot be normalized");
        return SizedFormat::Int32_R;
    case Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return SizedFormat::Float16_R;
    case Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return SizedFormat::Float32_R;
    default:
        throw std::runtime_error("Incorrect R format");
    }
}

inline SizedFormat GetRGSizedformat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return normalizedType ? SizedFormat::Uint8_NormalizedRG : SizedFormat::Uint8_RG;
    case Type::Int8:
        return normalizedType ? SizedFormat::Int8_NormalizedRG : SizedFormat::Int8_RG;
    case Type::Uint16:
        return normalizedType ? SizedFormat::Uint16_NormalizedRG : SizedFormat::Uint16_RG;
    case Type::Int16:
        return normalizedType ? SizedFormat::Int16_NormalizedRG : SizedFormat::Int16_RG;
    case Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return SizedFormat::Uint32_RG;
    case Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return SizedFormat::Int32_RG;
    case Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return SizedFormat::Float16_RG;
    case Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return SizedFormat::Float32_RG;
    default:
        throw std::runtime_error("Incorrect RG format");
    }
}

inline SizedFormat GetRGBSizedformat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return normalizedType ? SizedFormat::Uint8_NormalizedRGB : SizedFormat::Uint8_RGB;
    case Type::Int8:
        return normalizedType ? SizedFormat::Int8_NormalizedRGB : SizedFormat::Int8_RGB;
    case Type::Uint16:
        return normalizedType ? SizedFormat::Uint16_NormalizedRGB : SizedFormat::Uint16_RGB;
    case Type::Int16:
        return normalizedType ? SizedFormat::Int16_NormalizedRGB : SizedFormat::Int16_RGB;
    case Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return SizedFormat::Uint32_RGB;
    case Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return SizedFormat::Int32_RGB;
    case Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return SizedFormat::Float16_RGB;
    case Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return SizedFormat::Float32_RGB;
    default:
        throw std::runtime_error("Incorrect RGB format");
    }
}

inline SizedFormat GetRGBASizedFormat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return normalizedType ? SizedFormat::Uint8_NormalizedRGBA : SizedFormat::Uint8_RGBA;
    case Type::Int8:
        return normalizedType ? SizedFormat::Int8_NormalizedRGBA : SizedFormat::Int8_RGBA;
    case Type::Uint16:
        return normalizedType ? SizedFormat::Uint16_NormalizedRGBA : SizedFormat::Uint16_RGBA;
    case Type::Int16:
        return normalizedType ? SizedFormat::Int16_NormalizedRGBA : SizedFormat::Int16_RGBA;
    case Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return SizedFormat::Uint32_RGBA;
    case Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return SizedFormat::Int32_RGBA;
    case Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return SizedFormat::Float16_RGBA;
    case Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return SizedFormat::Float32_RGBA;
    case Type::DXT5Block:
        assert(normalizedType && "DXT5 pixel type must be normalized");
        return SizedFormat::DXT5_RGBA;
    default:
        throw std::runtime_error("Incorrect RGBA format");
    }
}

SizedFormat GetDepthSizedformat(Type type, bool normalizedType)
{
    assert(normalizedType);
    switch (type) {
    case Type::Uint32:
        return SizedFormat::Depth32;
    case Type::Float16:
        return SizedFormat::Depth16;
    case Type::Float32:
        return SizedFormat::Depth32F;
    default:
        throw std::runtime_error("Incorrect Depth format");
    }
    return SizedFormat::Unknown;
}

SizedFormat GetDepthStencilSizedFormat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint32:
        return SizedFormat::Depth24_Stencil8;
        break;
    case Type::Float32:
        return SizedFormat::Depth32F_Stencil8;
        break;
    default:
        throw std::runtime_error("Incorrect DepthStencil format");
    }
    return SizedFormat::Unknown;
}

SizedFormat GetStencilSizedFormat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return SizedFormat::Stencil8;
        break;
    case Type::Int8:
        assert(true && "Stencil texture cannot be of type Int8");
        break;
    case Type::Uint16:
        assert(true && "Stencil texture cannot be of type Uint16");
        break;
    case Type::Int16:
        assert(true && "Stencil texture cannot be of type Int16");
        break;
    case Type::Uint32:
        assert(true && "Stencil texture cannot be of type Uint32");
        break;
    case Type::Int32:
        assert(true && "Stencil texture cannot be of type Int32");
        break;
    case Type::Float16:
        assert(true && "Stencil texture cannot be of type Float16");
        break;
    case Type::Float32:
        assert(true && "Stencil texture cannot be of type Float32");
        break;
    default:
        throw std::runtime_error("Unknown Stencil format");
    }
    return SizedFormat::Unknown;
}

glm::vec4 LinearToSRGB(const glm::vec4& color)
{
    const glm::vec3 linearRGB { color.r, color.g, color.b };
    glm::bvec3 cutoff = lessThan(linearRGB, glm::vec3(0.0031308));
    glm::vec3 higher = glm::vec3(1.055) * pow(linearRGB, glm::vec3(1.0 / 2.4)) - glm::vec3(0.055);
    glm::vec3 lower = linearRGB * glm::vec3(12.92);

    return glm::vec4(mix(higher, lower, cutoff), color.a);
}

glm::vec4 BilinearFilter(const float& tx, const float& ty, const glm::vec4& c00, const glm::vec4& c10, const glm::vec4& c01, const glm::vec4& c11)
{
    auto  a = c00 * (1 - tx) + c10 * tx;
    auto  b = c01 * (1 - tx) + c11 * tx;
    return a * (1 - ty) + b * ty;
}

SizedFormat GetSizedFormat(UnsizedFormat unsizedFormat, Type type, bool normalized)
{
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
    default:
        throw std::runtime_error("Unknown Pixel::UnsizedFormat/Type");
    }
}

uint8_t GetUnsizedFormatComponentsNbr(UnsizedFormat format)
{
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
    default:
        throw std::runtime_error("Unknown Pixel::UnsizedFormat");
    }
}

uint8_t GetTypeSize(Type type)
{
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
    case Type::DXT5Block:
        return 16;
    default:
        throw std::runtime_error("Unknown Pixel::Type");
    }
}

uint8_t GetOctetsPerPixels(UnsizedFormat format, Type Type)
{
    return GetUnsizedFormatComponentsNbr(format) * GetTypeSize(Type);
}

Description::Description(UnsizedFormat format, Type type, bool normalized)
    : Description(Pixel::GetSizedFormat(format, type, normalized))
{
    /*_sizedFormat(GetSizedFormat(format, type, normalized))
    , _unsizedFormat(format)
    , _type(type)
    , _typeSize(GetTypeSize(type))
    , _components(GetUnsizedFormatComponentsNbr(format))
    , _size(_components * _typeSize)
    , _normalized(normalized)*/
}

Description::Description(SizedFormat format)
{
    assert(format != SizedFormat::Unknown);
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
    case SizedFormat::DXT5_RGBA:
        _unsizedFormat = UnsizedFormat::RGBA;
        _type = Type::DXT5Block;
        break;
    }
    _sizedFormat = format;
    _typeSize = Pixel::GetTypeSize(_type);
    _components = GetUnsizedFormatComponentsNbr(_unsizedFormat);
    _size = _components * _typeSize;
}

static inline float GetNormalizedColorComponent(Type type, const std::byte* bytes)
{
    assert(type != Type::Unknown);
    assert(type != Type::Uint32 && "Uint32 textures cannot be normalized");
    assert(type != Type::Int32 && "Int32 textures cannot be normalized");
    assert(type != Type::Float16 && "Float16 textures cannot be normalized");
    assert(type != Type::Float32 && "Float32 textures cannot be normalized");
    switch (type) {
    case Type::Uint8:
        return *reinterpret_cast<const uint8_t*>(bytes) / float(UINT8_MAX);
    case Type::Int8:
        return *reinterpret_cast<const int8_t*>(bytes) / float(INT8_MAX);
    case Type::Uint16:
        return *reinterpret_cast<const uint16_t*>(bytes) / float(UINT16_MAX);
    case Type::Int16:
        return *reinterpret_cast<const int16_t*>(bytes) / float(INT16_MAX);
    default:
        throw std::runtime_error("Cannot fetch color for this pixel type");
    }
    return 0;
}

static inline float GetColorComponent(Type type, const std::byte* bytes)
{
    assert(type != Type::Unknown);
    switch (type) {
    case Type::Uint8:
        return *reinterpret_cast<const uint8_t*>(bytes);
    case Type::Int8:
        return *reinterpret_cast<const int8_t*>(bytes);
    case Type::Uint16:
        return *reinterpret_cast<const uint16_t*>(bytes);
    case Type::Int16:
        return *reinterpret_cast<const int16_t*>(bytes);
    case Type::Uint32:
        return *reinterpret_cast<const uint32_t*>(bytes);
    case Type::Int32:
        return *reinterpret_cast<const int32_t*>(bytes);
    case Type::Float16:
        return glm::detail::toFloat32(*reinterpret_cast<const glm::detail::hdata*>(bytes));
    case Type::Float32:
        return *reinterpret_cast<const float*>(bytes);
    default:
        throw std::runtime_error("Cannot fetch color for this pixel type");
    }
    return 0;
}

Color Description::GetColorFromBytes(const std::byte* bytes) const
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
    default:
        throw std::runtime_error("Incorrect pixel type");
    }
    return color;
}

static inline void SetComponentNormalized(Type type, std::byte* bytes, float component)
{
    assert(type != Type::Unknown);
    assert(type != Type::Uint32 && "Uint32 textures cannot be normalized");
    assert(type != Type::Int32 && "Int32 textures cannot be normalized");
    assert(type != Type::Float16 && "Float16 textures cannot be normalized");
    assert(type != Type::Float32 && "Float32 textures cannot be normalized");
    switch (type) {
    case Type::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = glm::clamp(component, 0.f, 1.f) * float(UINT8_MAX);
        break;
    case Type::Int8:
        *reinterpret_cast<int8_t*>(bytes) = glm::clamp(component, -1.f, 1.f) * float(INT8_MAX);
        break;
    case Type::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = glm::clamp(component, 0.f, 1.f) * float(UINT16_MAX);
        break;
    case Type::Int16:
        *reinterpret_cast<int16_t*>(bytes) = glm::clamp(component, -1.f, 1.f) * float(INT16_MAX);
        break;
    default:
        throw std::runtime_error("Cannot set color for this pixel type");
    }
}

static inline void SetComponent(Type type, std::byte* bytes, float component)
{
    assert(type != Type::Unknown);
    switch (type) {
    case Type::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = component;
        break;
    case Type::Int8:
        *reinterpret_cast<int8_t*>(bytes) = component;
        break;
    case Type::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = component;
        break;
    case Type::Int16:
        *reinterpret_cast<int16_t*>(bytes) = component;
        break;
    case Type::Uint32:
        *reinterpret_cast<uint32_t*>(bytes) = component;
        break;
    case Type::Int32:
        *reinterpret_cast<int32_t*>(bytes) = component;
        break;
    case Type::Float16:
        *reinterpret_cast<glm::detail::hdata*>(bytes) = glm::detail::toFloat16(component);
        break;
    case Type::Float32:
        *reinterpret_cast<float*>(bytes) = component;
        break;
    default:
        throw std::runtime_error("Cannot set color for this pixel type");
    }
}

void Description::SetColorToBytes(std::byte* bytes, const Color& color) const
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
}

