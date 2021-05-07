/*
* @Author: gpinchon
* @Date:   2021-05-04 22:26:27
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 22:32:24
*/
#pragma once
#include "Texture/PixelUtils.hpp"

namespace OpenGL {
namespace Pixel {
    unsigned GetGLEnum(::Pixel::SizedFormat);
    unsigned GetGLEnum(::Pixel::UnsizedFormat);
    unsigned GetGLEnum(::Pixel::Type);
}
}