/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:42:10
*/

#include "Texture/Texture.hpp"
//#include "Buffer/Buffer.hpp"
//#include "Config.hpp" // for Config
#include "Debug.hpp" // for glCheckError, debugLog
#include <algorithm> // for min
#include <cstring> // for memcpy
#include <stdint.h> // for int16_t, uint64_t
#include <utility> // for pair

Texture::Texture(Texture::Type target, Pixel::Description pixelDesc)
    : Component()
{
    _SetType(target);
    SetPixelDescription(pixelDesc);
}

Texture::Texture(Texture::Type target)
{
    _SetType(target);
}

Texture::~Texture()
{
    Unload();
}

void Texture::RestoreParameters()
{
    for (const auto parameterf : _parametersf)
        SetParameter(parameterf.first, parameterf.second);
    for (const auto parameteri : _parametersi)
        SetParameter(parameteri.first, parameteri.second);
}

void Texture::Unload()
{
    GLuint id = GetHandle();
    _SetHandle(0);
    glDeleteTextures(1, &id);
    _SetLoaded(false);
}

void Texture::GenerateMipmap()
{
    glGenerateTextureMipmap(GetHandle());
    //SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void Texture::SetPixelDescription(Pixel::Description pixelDesc)
{
    _SetPixelDescription(pixelDesc);
    _SetLoaded(false);
}

Texture::Handle Texture::Create(Texture::Type type)
{
    GLuint glid;
    glCreateTextures((GLenum)type, 1, &glid);
    return glid;
}
