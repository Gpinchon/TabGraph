/*
* @Author: gpinchon
* @Date:   2021-01-12 18:26:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-12 22:44:04
*/
#pragma once

#include <GL/glew.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <glm/vec4.hpp>

namespace Pixel {
glm::vec4 LinearToSRGB(glm::vec4 color);

enum class SizedFormat : GLenum {
    Unknown = -1,
    /// <summary>
    /// Normalized Uint8 pixel types
    /// </summary>
    Uint8_NormalizedR = GL_R8,
    Uint8_NormalizedRG = GL_RG8,
    Uint8_NormalizedRGB = GL_RGB8,
    Uint8_NormalizedRGBA = GL_RGBA8,
    /// <summary>
    /// Normalized Int8 pixel types
    /// </summary>
    Int8_NormalizedR = GL_R8_SNORM,
    Int8_NormalizedRG = GL_RG8_SNORM,
    Int8_NormalizedRGB = GL_RGB8_SNORM,
    Int8_NormalizedRGBA = GL_RGBA8_SNORM,
    /// <summary>
    /// Unnormalized Uint8 pixel types
    /// </summary>
    Uint8_R = GL_R8UI,
    Uint8_RG = GL_RG8UI,
    Uint8_RGB = GL_RGB8UI,
    Uint8_RGBA = GL_RGBA8UI,
    /// <summary>
    /// Unnormalized Int8 pixel types
    /// </summary>
    Int8_R = GL_R8I,
    Int8_RG = GL_RG8I,
    Int8_RGB = GL_RGB8I,
    Int8_RGBA = GL_RGBA8I,
    /// <summary>
    /// Normalized Uint16 pixel types
    /// </summary>
    Uint16_NormalizedR = GL_R16,
    Uint16_NormalizedRG = GL_RG16,
    Uint16_NormalizedRGB = GL_RGB16,
    Uint16_NormalizedRGBA = GL_RGBA16,
    /// <summary>
    /// Normalized Int16 pixel types
    /// </summary>
    Int16_NormalizedR = GL_R16_SNORM,
    Int16_NormalizedRG = GL_RG16_SNORM,
    Int16_NormalizedRGB = GL_RGB16_SNORM,
    Int16_NormalizedRGBA = GL_RGBA16_SNORM,
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Uint16_R = GL_R16UI,
    Uint16_RG = GL_RG16UI,
    Uint16_RGB = GL_RGB16UI,
    Uint16_RGBA = GL_RGBA16UI,
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Int16_R = GL_R16I,
    Int16_RG = GL_RG16I,
    Int16_RGB = GL_RGB16I,
    Int16_RGBA = GL_RGBA16I,
    /// <summary>
    /// Uint32 pixel types
    /// </summary>
    Uint32_R = GL_R32UI,
    Uint32_RG = GL_RG32UI,
    Uint32_RGB = GL_RGB32UI,
    Uint32_RGBA = GL_RGBA32UI,
    /// <summary>
    /// Int32 pixel types
    /// </summary>
    Int32_R = GL_R32I,
    Int32_RG = GL_RG32I,
    Int32_RGB = GL_RGB32I,
    Int32_RGBA = GL_RGBA32I,
    /// <summary>
    /// Floating Point pixel types
    /// </summary>
    Float16_R = GL_R16F,
    Float16_RG = GL_RG16F,
    Float16_RGB = GL_RGB16F,
    Float16_RGBA = GL_RGBA16F,
    Float32_R = GL_R32F,
    Float32_RG = GL_RG32F,
    Float32_RGB = GL_RGB32F,
    Float32_RGBA = GL_RGBA32F,

    Depth16 = GL_DEPTH_COMPONENT16,
    Depth24 = GL_DEPTH_COMPONENT24,
    Depth32 = GL_DEPTH_COMPONENT32,
    Depth32F = GL_DEPTH_COMPONENT32F,

    Depth24_Stencil8 = GL_DEPTH24_STENCIL8,
    Depth32F_Stencil8 = GL_DEPTH32F_STENCIL8,
    Stencil8 = GL_STENCIL_INDEX8,
    MaxValue
};

enum class UnsizedFormat : GLenum {
    Unknown = -1,
    R = GL_RED,
    RG = GL_RG,
    RGB = GL_RGB,
    RGBA = GL_RGBA,
    R_Integer = GL_RED_INTEGER,
    RG_Integer = GL_RG_INTEGER,
    RGB_Integer = GL_RGB_INTEGER,
    RGBA_Integer = GL_RGBA_INTEGER,
    Depth = GL_DEPTH_COMPONENT,
    Depth_Stencil = GL_DEPTH_STENCIL,
    Stencil = GL_STENCIL
};

enum class Type : GLenum {
    Unknown = -1,
    Uint8 = GL_UNSIGNED_BYTE,
    Int8 = GL_BYTE,
    Uint16 = GL_UNSIGNED_SHORT,
    Int16 = GL_SHORT,
    Uint32 = GL_UNSIGNED_INT,
    Int32 = GL_INT,
    Float16 = GL_HALF_FLOAT,
    Float32 = GL_FLOAT
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
    SizedFormat _sizedFormat{ Pixel::SizedFormat::Unknown };
    UnsizedFormat _unsizedFormat{ Pixel::UnsizedFormat::Unknown };
    Type _type{ Pixel::Type::Unknown };
    uint8_t _typeSize{ 0 };
    uint8_t _components{ 0 };
    uint8_t _size{ 0 };
    bool _normalized{ false };
    bool _hasAlpha{ false };
};
}