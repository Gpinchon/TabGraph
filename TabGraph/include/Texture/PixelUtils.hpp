/*
* @Author: gpinchon
* @Date:   2021-01-12 18:26:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-29 18:21:49
*/
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <glm/vec4.hpp>

namespace Pixel {
glm::vec4 LinearToSRGB(const glm::vec4& color);
/**
 * @brief Performes bilinear filtering on the supplied colors
 * @param tx : texture coordinates fract on x axis
 * @param ty : texture coordinates fract on y axis
 * @param c00 : color at pixel [x, y]
 * @param c10 : color at pixel [x + 1, y]
 * @param c01 : color at pixel [x, y + 1]
 * @param c11 : color at pixel [x + 1, y + 1]
 * @return 
*/
glm::vec4 BilinearFilter(
    const float& tx,
    const float& ty,
    const glm::vec4& c00,
    const glm::vec4& c10,
    const glm::vec4& c01,
    const glm::vec4& c11);

enum class SizedFormat {
    Unknown = -1,
    /// <summary>
    /// Normalized Uint8 pixel types
    /// </summary>
    Uint8_NormalizedR,
    Uint8_NormalizedRG,
    Uint8_NormalizedRGB,
    Uint8_NormalizedRGBA,
    /// <summary>
    /// Normalized Int8 pixel types
    /// </summary>
    Int8_NormalizedR,
    Int8_NormalizedRG,
    Int8_NormalizedRGB,
    Int8_NormalizedRGBA,
    /// <summary>
    /// Unnormalized Uint8 pixel types
    /// </summary>
    Uint8_R,
    Uint8_RG,
    Uint8_RGB,
    Uint8_RGBA,
    /// <summary>
    /// Unnormalized Int8 pixel types
    /// </summary>
    Int8_R,
    Int8_RG,
    Int8_RGB,
    Int8_RGBA,
    /// <summary>
    /// Normalized Uint16 pixel types
    /// </summary>
    Uint16_NormalizedR,
    Uint16_NormalizedRG,
    Uint16_NormalizedRGB,
    Uint16_NormalizedRGBA,
    /// <summary>
    /// Normalized Int16 pixel types
    /// </summary>
    Int16_NormalizedR,
    Int16_NormalizedRG,
    Int16_NormalizedRGB,
    Int16_NormalizedRGBA,
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Uint16_R,
    Uint16_RG,
    Uint16_RGB,
    Uint16_RGBA,
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Int16_R,
    Int16_RG,
    Int16_RGB,
    Int16_RGBA,
    /// <summary>
    /// Uint32 pixel types
    /// </summary>
    Uint32_R,
    Uint32_RG,
    Uint32_RGB,
    Uint32_RGBA,
    /// <summary>
    /// Int32 pixel types
    /// </summary>
    Int32_R,
    Int32_RG,
    Int32_RGB,
    Int32_RGBA,
    /// <summary>
    /// Floating Point pixel types
    /// </summary>
    Float16_R,
    Float16_RG,
    Float16_RGB,
    Float16_RGBA,
    Float32_R,
    Float32_RG,
    Float32_RGB,
    Float32_RGBA,
    /// <summary>
    /// Depth pixel types
    /// </summary>
    Depth16,
    Depth24,
    Depth32,
    Depth32F,
    /// <summary>
    /// Depth/Stencil pixel types
    /// </summary>
    Depth24_Stencil8,
    Depth32F_Stencil8,
    Stencil8,
    /// <summary>
    /// Compressed pixel types
    /// </summary>
    DXT5_RGBA,
    MaxValue
};

enum class UnsizedFormat {
    Unknown = -1,
    R,
    RG,
    RGB,
    RGBA,
    R_Integer,
    RG_Integer,
    RGB_Integer,
    RGBA_Integer,
    Depth,
    Depth_Stencil,
    Stencil,
    MaxValue
};

enum class Type {
    Unknown = -1,
    Uint8,
    Int8,
    Uint16,
    Int16,
    Uint32,
    Int32,
    Float16,
    Float32,
    DXT5Block,
    MaxValue
};

using Color = glm::vec4;

SizedFormat GetSizedFormat(UnsizedFormat unsizedFormat, Type type, bool normalized = false);

uint8_t GetUnsizedFormatComponentsNbr(UnsizedFormat format);

uint8_t GetTypeSize(Type Type);

uint8_t GetOctetsPerPixels(UnsizedFormat format, Type Type);

struct Description {
    Description() = default;
    Description(UnsizedFormat format, Type type, bool normalized = false);
    Description(SizedFormat format);
    SizedFormat GetSizedFormat() const;
    UnsizedFormat GetUnsizedFormat() const;
    /** @return the data type */
    Type GetType() const;
    /**
     * @brief 
     * @return the data type size in octets
    */
    uint8_t GetTypeSize() const;
    /**
     * @brief 
     * @return the number of components
    */
    uint8_t GetComponents() const;
    /**
     * @brief 
     * @return the pixel size in octets
    */
    uint8_t GetSize() const;
    /**
     * @brief 
     * @return true if the pixel is to be normalized 
    */
    bool GetNormalized() const;
    /**
     * @brief Converts raw bytes to float RGBA representation
     * @param bytes the raw bytes to be converted
     * @return the unpacked color
    */
    Color GetColorFromBytes(std::byte* bytes) const;
    /**
     * @brief Writes color to the raw bytes
     * @param bytes the raw bytes to write to
     * @param color the color to write
    */
    void SetColorToBytes(std::byte* bytes, const Color& color) const;
    bool operator==(const Description& other) const
    {
        return _sizedFormat == other._sizedFormat
            && _unsizedFormat == other._unsizedFormat
            && _type == other._type;
    }
    bool operator!=(const Description& other) const
    {
        return !((*this) == other);
    }
    bool operator!=(Description& other) const
    {
        return !((*this) == other);
    }

private:
    SizedFormat _sizedFormat { Pixel::SizedFormat::Unknown };
    UnsizedFormat _unsizedFormat { Pixel::UnsizedFormat::Unknown };
    Type _type { Pixel::Type::Unknown };
    uint8_t _typeSize { 0 };
    uint8_t _components { 0 };
    uint8_t _size { 0 };
    bool _normalized { false };
    bool _hasAlpha { false };
};
}