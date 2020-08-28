/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-26 13:06:13
*/

#include "Texture/Texture.hpp"
#include "Config.hpp" // for Config
#include "Debug.hpp" // for glCheckError, debugLog
#include <algorithm> // for min
#include <cstring> // for memcpy
#include <stdint.h> // for int16_t, uint64_t
#include <utility> // for pair

Texture::Texture(const std::string& name)
    : Object(name)
{
}

size_t Texture::get_bpp(GLenum texture_format, GLenum data_format)
{
    auto data_size = Texture::get_data_size(data_format);
    switch (texture_format) {
    case GL_RED:
        return (1 * 8 * data_size);
    case GL_RG:
        return (2 * 8 * data_size);
    case GL_RGB:
    case GL_BGR:
        return (3 * 8 * data_size);
    case GL_RGBA:
    case GL_BGRA:
        return (4 * 8 * data_size);
    default:
        return (0);
    }
}

size_t Texture::get_data_size(GLenum data_format)
{
    switch (data_format) {
    case GL_FLOAT:
    case GL_FIXED:
    case GL_INT:
    case GL_UNSIGNED_INT:
        return (sizeof(GLfloat));
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return (sizeof(GLubyte));
    case GL_HALF_FLOAT:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return (sizeof(GLushort));
    case GL_DOUBLE:
        return (sizeof(GLdouble));
    default:
        return (0);
    }
}

void Texture::SetTarget(GLenum target)
{
    _target = target;
}

GLenum Texture::target() const
{
    return (_target);
}

void* Texture::data() const
{
    return (_data);
}

void Texture::unload()
{
    if (!_loaded) {
        return;
    }
    glDeleteTextures(1, &_glid);
    _loaded = false;
    _glid = 0u;
}

void Texture::load()
{
    if (_loaded)
        return;
    debugLog(Name());
    if (glid() != 0u)
        glDeleteTextures(1, &_glid);
    if (glCreateTextures == nullptr) {
        glGenTextures(1, &_glid);
        glBindTexture(target(), glid());
        glBindTexture(target(), 0);
        if (glCheckError(Name()))
            throw std::runtime_error("Error while generating Texture");
    } else {
        glCreateTextures(target(), 1, &_glid);
        if (glCheckError(Name()))
            throw std::runtime_error("Error while generating Texture");
    }
    glObjectLabel(GL_TEXTURE, glid(), Name().length(), Name().c_str());
    if (glCheckError(Name()))
        throw std::runtime_error("Error while setting object label");
    _loaded = true;
    debugLog(Name() + " : Done.");
}

void Texture::generate_mipmap()
{
    glBindTexture(_target, _glid);
    if (glCheckError(Name()))
        throw std::runtime_error("");
    glGenerateMipmap(_target);
    if (glCheckError(Name()))
        throw std::runtime_error("");
    glBindTexture(_target, 0);
    if (glCheckError(Name()))
        throw std::runtime_error("");
    _mipMapsGenerated = true;
}

void Texture::format(GLenum* format, GLenum* internal_format)
{
    *format = _format;
    *internal_format = _internal_format;
}

GLubyte Texture::bpp() const
{
    return (_bpp);
}

GLuint Texture::glid() const
{
    return (_glid);
}

void Texture::SetInternalFormat(GLenum internalFormat)
{
    _internal_format = internalFormat;
}

GLenum Texture::InternalFormat()
{
    return (_internal_format);
}

GLenum Texture::format()
{
    return (_format);
}

GLenum Texture::data_format()
{
    return (_data_format);
}

size_t Texture::data_size()
{
    return (_data_size);
}

size_t Texture::values_per_pixel()
{
    return (_data_size ? _bpp / _data_size / 8 : 0);
}

void Texture::set_parameterf(GLenum p, float v)
{
    _parametersf[p] = v;
    if (_glid == 0u)
        return;
    if (glTextureParameterf == nullptr) {
        glBindTexture(_target, _glid);
        glTexParameterf(_target, p, v);
        glBindTexture(_target, 0);
    } else {
        glTextureParameterf(_glid, p, v);
    }
    if (glCheckError(Name()))
        throw std::runtime_error("Error while setting float parameter");
}

void Texture::set_parameteri(GLenum p, int v)
{
    _parametersi[p] = v;
    if (_glid == 0u)
        return;
    if (glTextureParameteri == nullptr) {
        glBindTexture(_target, _glid);
        glTexParameteri(_target, p, v);
        glBindTexture(_target, 0);
    } else {
        glTextureParameteri(_glid, p, v);
    }
    if (glCheckError(Name()))
        throw std::runtime_error("Error while setting interger parameter");
}

void Texture::restore_parameters()
{
    for (auto p : _parametersi) {
        set_parameteri(p.first, p.second);
    }
    for (auto p : _parametersf) {
        set_parameterf(p.first, p.second);
    }
}

bool Texture::is_loaded()
{
    return (_loaded);
}
