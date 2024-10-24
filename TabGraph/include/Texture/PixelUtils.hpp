/*
* @Author: gpinchon
* @Date:   2021-01-12 18:26:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-29 18:21:49
*/
#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

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
    inline SizedFormat GetSizedFormat() const
    {
        return _sizedFormat;
    }
    inline UnsizedFormat GetUnsizedFormat() const
    {
        return _unsizedFormat;
    }
    /** @return the data type */
    inline Type GetType() const
    {
        return _type;
    }
    /**
     * @brief 
     * @return the data type size in octets
    */
    inline uint8_t GetTypeSize() const
    {
        return _typeSize;
    }
    /**
     * @brief 
     * @return the number of components
    */
    inline uint8_t GetComponents() const
    {
        return _components;
    }
    /**
     * @brief 
     * @return the pixel size in octets
    */
    inline uint8_t GetSize() const
    {
        return _size;
    }
    /**
     * @brief 
     * @return true if the pixel is to be normalized 
    */
    inline bool GetNormalized() const
    {
        return _normalized;
    }
    /**
     * @brief Converts raw bytes to float RGBA representation
     * @param bytes the raw bytes to be converted
     * @return the unpacked color
    */
    Color GetColorFromBytes(const std::byte* bytes) const;
    /**
     * @brief Get color from image's raw bytes
     * @param bytes : the image's raw bytes
     * @param imageSize : image's size in pixels
     * @param pixelCoordinates : the pixel to fetch
     * @return the color of the pixel located at textureCoordinates
    */
    inline Color GetColorFromBytes(const std::vector<std::byte>& bytes, const glm::ivec2& imageSize, const glm::ivec2& pixelCoordinates) const
    {
        auto pixelIndex{ GetPixelIndex(imageSize, pixelCoordinates) };
        auto pixelPtr{ &bytes.at(pixelIndex) };
        assert((pixelIndex + GetSize()) <= bytes.size() && "The pixel is out of bound");
        return GetColorFromBytes(pixelPtr);
    }
    /**
     * @brief Writes color to the raw bytes
     * @param bytes the raw bytes to write to
     * @param color the color to write
    */
    void SetColorToBytes(std::byte* bytes, const Color& color) const;
    /**
     * @brief Computes the pixel index at pixelCoordinates of an image with specified imageSize
     * @param imageSize : the size of the image in pixels
     * @param pixelCoordinates : the pixel's coordinates to fetch the index for
     * @return the pixel index, the DXT block index for DXT formats
    */
    inline size_t GetPixelIndex(const glm::ivec2& imageSize, const glm::ivec2& pixelCoordinates) const
    {
        auto unsizedPixelIndex = static_cast<size_t>(pixelCoordinates.y) * imageSize.x + pixelCoordinates.x;
        if (GetType() == Pixel::Type::DXT5Block) {
            //DXT5 compression format is composed of 4x4 pixels
            auto blockNumX = imageSize.x / 4;
            auto blockX = pixelCoordinates.x / 4;
            auto blockY = pixelCoordinates.y / 4;
            unsizedPixelIndex = static_cast<size_t>(blockY) * blockNumX + blockX;
        }
        return unsizedPixelIndex * GetSize();
    }
    bool operator==(const Description& other) const
    {
        return _sizedFormat == other._sizedFormat
            && _unsizedFormat == other._unsizedFormat
            && _type == other._type;
    }
    bool operator!=(const Description& other) const
    {
        return !(*this == other);
    }
    bool operator!=(Description& other) const
    {
        return !(*this == other);
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