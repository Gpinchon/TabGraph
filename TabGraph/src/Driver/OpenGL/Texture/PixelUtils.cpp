/*
* @Author: gpinchon
* @Date:   2021-05-04 22:26:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-29 16:27:08
*/

#include "Driver/OpenGL/Texture/PixelUtils.hpp"

#include <GL/glew.h>
#include <stdexcept>

namespace OpenGL {
unsigned GetEnum(::Pixel::SizedFormat format)
{
    switch (format) {
    case (::Pixel::SizedFormat::Uint8_NormalizedR):
        return GL_R8;
    case (::Pixel::SizedFormat::Uint8_NormalizedRG):
        return GL_RG8;
    case (::Pixel::SizedFormat::Uint8_NormalizedRGB):
        return GL_RGB8;
    case (::Pixel::SizedFormat::Uint8_NormalizedRGBA):
        return GL_RGBA8;
    case (::Pixel::SizedFormat::Int8_NormalizedR):
        return GL_R8_SNORM;
    case (::Pixel::SizedFormat::Int8_NormalizedRG):
        return GL_RG8_SNORM;
    case (::Pixel::SizedFormat::Int8_NormalizedRGB):
        return GL_RGB8_SNORM;
    case (::Pixel::SizedFormat::Int8_NormalizedRGBA):
        return GL_RGBA8_SNORM;
    case (::Pixel::SizedFormat::Uint8_R):
        return GL_R8UI;
    case (::Pixel::SizedFormat::Uint8_RG):
        return GL_RG8UI;
    case (::Pixel::SizedFormat::Uint8_RGB):
        return GL_RGB8UI;
    case (::Pixel::SizedFormat::Uint8_RGBA):
        return GL_RGBA8UI;
    case (::Pixel::SizedFormat::Int8_R):
        return GL_R8I;
    case (::Pixel::SizedFormat::Int8_RG):
        return GL_RG8I;
    case (::Pixel::SizedFormat::Int8_RGB):
        return GL_RGB8I;
    case (::Pixel::SizedFormat::Int8_RGBA):
        return GL_RGBA8I;
    case (::Pixel::SizedFormat::Uint16_NormalizedR):
        return GL_R16;
    case (::Pixel::SizedFormat::Uint16_NormalizedRG):
        return GL_RG16;
    case (::Pixel::SizedFormat::Uint16_NormalizedRGB):
        return GL_RGB16;
    case (::Pixel::SizedFormat::Uint16_NormalizedRGBA):
        return GL_RGBA16;
    case (::Pixel::SizedFormat::Int16_NormalizedR):
        return GL_R16_SNORM;
    case (::Pixel::SizedFormat::Int16_NormalizedRG):
        return GL_RG16_SNORM;
    case (::Pixel::SizedFormat::Int16_NormalizedRGB):
        return GL_RGB16_SNORM;
    case (::Pixel::SizedFormat::Int16_NormalizedRGBA):
        return GL_RGBA16_SNORM;
    case (::Pixel::SizedFormat::Uint16_R):
        return GL_R16UI;
    case (::Pixel::SizedFormat::Uint16_RG):
        return GL_RG16UI;
    case (::Pixel::SizedFormat::Uint16_RGB):
        return GL_RGB16UI;
    case (::Pixel::SizedFormat::Uint16_RGBA):
        return GL_RGBA16UI;
    case (::Pixel::SizedFormat::Int16_R):
        return GL_R16I;
    case (::Pixel::SizedFormat::Int16_RG):
        return GL_RG16I;
    case (::Pixel::SizedFormat::Int16_RGB):
        return GL_RGB16I;
    case (::Pixel::SizedFormat::Int16_RGBA):
        return GL_RGBA16I;
    case (::Pixel::SizedFormat::Uint32_R):
        return GL_R32UI;
    case (::Pixel::SizedFormat::Uint32_RG):
        return GL_RG32UI;
    case (::Pixel::SizedFormat::Uint32_RGB):
        return GL_RGB32UI;
    case (::Pixel::SizedFormat::Uint32_RGBA):
        return GL_RGBA32UI;
    case (::Pixel::SizedFormat::Int32_R):
        return GL_R32I;
    case (::Pixel::SizedFormat::Int32_RG):
        return GL_RG32I;
    case (::Pixel::SizedFormat::Int32_RGB):
        return GL_RGB32I;
    case (::Pixel::SizedFormat::Int32_RGBA):
        return GL_RGBA32I;
    case (::Pixel::SizedFormat::Float16_R):
        return GL_R16F;
    case (::Pixel::SizedFormat::Float16_RG):
        return GL_RG16F;
    case (::Pixel::SizedFormat::Float16_RGB):
        return GL_RGB16F;
    case (::Pixel::SizedFormat::Float16_RGBA):
        return GL_RGBA16F;
    case (::Pixel::SizedFormat::Float32_R):
        return GL_R32F;
    case (::Pixel::SizedFormat::Float32_RG):
        return GL_RG32F;
    case (::Pixel::SizedFormat::Float32_RGB):
        return GL_RGB32F;
    case (::Pixel::SizedFormat::Float32_RGBA):
        return GL_RGBA32F;
    case (::Pixel::SizedFormat::Depth16):
        return GL_DEPTH_COMPONENT16;
    case (::Pixel::SizedFormat::Depth24):
        return GL_DEPTH_COMPONENT24;
    case (::Pixel::SizedFormat::Depth32):
        return GL_DEPTH_COMPONENT32;
    case (::Pixel::SizedFormat::Depth32F):
        return GL_DEPTH_COMPONENT32F;
    case (::Pixel::SizedFormat::Depth24_Stencil8):
        return GL_DEPTH24_STENCIL8;
    case (::Pixel::SizedFormat::Depth32F_Stencil8):
        return GL_DEPTH32F_STENCIL8;
    case (::Pixel::SizedFormat::Stencil8):
        return GL_STENCIL_INDEX8;
    case (::Pixel::SizedFormat::DXT5_RGBA):
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    default:
        throw std::runtime_error("Unknown Pixel::SizedFormat");
    }
}

unsigned GetEnum(::Pixel::UnsizedFormat format)
{
    switch (format) {
    case (::Pixel::UnsizedFormat::R):
        return GL_RED;
    case (::Pixel::UnsizedFormat::RG):
        return GL_RG;
    case (::Pixel::UnsizedFormat::RGB):
        return GL_RGB;
    case (::Pixel::UnsizedFormat::RGBA):
        return GL_RGBA;
    case (::Pixel::UnsizedFormat::R_Integer):
        return GL_RED_INTEGER;
    case (::Pixel::UnsizedFormat::RG_Integer):
        return GL_RG_INTEGER;
    case (::Pixel::UnsizedFormat::RGB_Integer):
        return GL_RGB_INTEGER;
    case (::Pixel::UnsizedFormat::RGBA_Integer):
        return GL_RGBA_INTEGER;
    case (::Pixel::UnsizedFormat::Depth):
        return GL_DEPTH_COMPONENT;
    case (::Pixel::UnsizedFormat::Depth_Stencil):
        return GL_DEPTH_STENCIL;
    case (::Pixel::UnsizedFormat::Stencil):
        return GL_STENCIL;
    default:
        throw std::runtime_error("Unknown Pixel::UnsizedFormat");
    }
}

unsigned GetEnum(::Pixel::Type type)
{
    switch (type) {
    case (::Pixel::Type::Uint8):
        return GL_UNSIGNED_BYTE;
    case (::Pixel::Type::Int8):
        return GL_BYTE;
    case (::Pixel::Type::Uint16):
        return GL_UNSIGNED_SHORT;
    case (::Pixel::Type::Int16):
        return GL_SHORT;
    case (::Pixel::Type::Uint32):
        return GL_UNSIGNED_INT;
    case (::Pixel::Type::Int32):
        return GL_INT;
    case (::Pixel::Type::Float16):
        return GL_HALF_FLOAT;
    case (::Pixel::Type::Float32):
        return GL_FLOAT;
    default:
        throw std::runtime_error("Unknown Pixel::Type");
    }
}
};