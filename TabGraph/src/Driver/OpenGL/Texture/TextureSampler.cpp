/*
* @Author: gpinchon
* @Date:   2021-05-01 20:04:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-03 17:23:39
*/

#include "Driver/OpenGL/Texture/TextureSampler.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

static inline auto GetWrapEnum(GLenum value)
{
    switch (value) {
    case GL_REPEAT:
        return TextureSampler::Wrap::Repeat;
    case GL_CLAMP_TO_BORDER:
        return TextureSampler::Wrap::ClampToBorder;
    case GL_CLAMP_TO_EDGE:
        return TextureSampler::Wrap::ClampToEdge;
    case GL_MIRRORED_REPEAT:
        return TextureSampler::Wrap::MirroredRepeat;
    case GL_MIRROR_CLAMP_TO_EDGE:
        return TextureSampler::Wrap::MirroredClampToEdge;
    }
}

static inline auto GetFilterEnum(GLenum value)
{
    switch (value) {
    case GL_NEAREST:
        return TextureSampler::Filter::Nearest;
    case GL_LINEAR:
        return TextureSampler::Filter::Linear;
    case GL_NEAREST_MIPMAP_LINEAR:
        return TextureSampler::Filter::NearestMipmapLinear;
    case GL_NEAREST_MIPMAP_NEAREST:
        return TextureSampler::Filter::NearestMipmapNearest;
    case GL_LINEAR_MIPMAP_LINEAR:
        return TextureSampler::Filter::LinearMipmapLinear;
    case GL_LINEAR_MIPMAP_NEAREST:
        return TextureSampler::Filter::LinearMipmapNearest;
    }
}

static inline auto GetCompareModeEnum(GLenum value)
{
    switch (value) {
    case GL_NONE:
        return TextureSampler::CompareMode::None;
    case GL_COMPARE_REF_TO_TEXTURE:
        return TextureSampler::CompareMode::CompareRefToTexture;
    }
}

static inline auto GetCompareFuncEnum(GLenum value)
{
    switch (value) {
    case GL_LEQUAL:
        return TextureSampler::CompareFunc::LessEqual;
    case GL_GEQUAL:
        return TextureSampler::CompareFunc::GreaterEqual;
    case GL_LESS:
        return TextureSampler::CompareFunc::Less;
    case GL_GREATER:
        return TextureSampler::CompareFunc::Greater;
    case GL_EQUAL:
        return TextureSampler::CompareFunc::Equal;
    case GL_NOTEQUAL:
        return TextureSampler::CompareFunc::NotEqual;
    case GL_ALWAYS:
        return TextureSampler::CompareFunc::Always;
    case GL_NEVER:
        return TextureSampler::CompareFunc::Never;
    }
}

static inline auto GetGLEnum(TextureSampler::Wrap value)
{
    switch (value) {
    case TextureSampler::Wrap::Repeat:
        return GL_REPEAT;
    case TextureSampler::Wrap::ClampToBorder:
        return GL_CLAMP_TO_BORDER;
    case TextureSampler::Wrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case TextureSampler::Wrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    case TextureSampler::Wrap::MirroredClampToEdge:
        return GL_MIRROR_CLAMP_TO_EDGE;
    }
}

static inline auto GetGLEnum(TextureSampler::Filter value)
{
    switch (value) {
    case TextureSampler::Filter::Nearest:
        return GL_NEAREST;
    case TextureSampler::Filter::Linear:
        return GL_LINEAR;
    case TextureSampler::Filter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case TextureSampler::Filter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case TextureSampler::Filter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case TextureSampler::Filter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    }
}

static inline auto GetGLEnum(TextureSampler::CompareMode value)
{
    switch (value) {
    case TextureSampler::CompareMode::None:
        return GL_NONE;
    case TextureSampler::CompareMode::CompareRefToTexture:
        return GL_COMPARE_REF_TO_TEXTURE;
    }
}

static inline auto GetGLEnum(TextureSampler::CompareFunc value)
{
    switch (value) {
    case TextureSampler::CompareFunc::LessEqual:
        return GL_LEQUAL;
    case TextureSampler::CompareFunc::GreaterEqual:
        return GL_GEQUAL;
    case TextureSampler::CompareFunc::Less:
        return GL_LESS;
    case TextureSampler::CompareFunc::Greater:
        return GL_GREATER;
    case TextureSampler::CompareFunc::Equal:
        return GL_EQUAL;
    case TextureSampler::CompareFunc::NotEqual:
        return GL_NOTEQUAL;
    case TextureSampler::CompareFunc::Always:
        return GL_ALWAYS;
    case TextureSampler::CompareFunc::Never:
        return GL_NEVER;
    }
}

TextureSampler::Impl::Impl()
{
    glGenSamplers(1, &_handle);
}

TextureSampler::Impl::~Impl()
{
    glDeleteSamplers(1, &_handle);
}

const TextureSampler::Handle& TextureSampler::Impl::GetHandle() const
{
    return _handle;
}

TextureSampler::Filter TextureSampler::Impl::GetMagFilter() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_MAG_FILTER, &value);
    return GetFilterEnum(value);
}

TextureSampler::Filter TextureSampler::Impl::GetMinFilter() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_MIN_FILTER, &value);
    return GetFilterEnum(value);
}

float TextureSampler::Impl::GetMinLOD() const
{
    float value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_MIN_LOD, &value);
    return value;
}

float TextureSampler::Impl::GetMaxLOD() const
{
    float value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_MAX_LOD, &value);
    return value;
}

float TextureSampler::Impl::GetLODBias() const
{
    float value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_LOD_BIAS, &value);
    return value;
}

TextureSampler::Wrap TextureSampler::Impl::GetWrapS() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_WRAP_S, &value);
    return GetWrapEnum(value);
}

TextureSampler::Wrap TextureSampler::Impl::GetWrapT() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_WRAP_T, &value);
    return GetWrapEnum(value);
}

TextureSampler::Wrap TextureSampler::Impl::GetWrapR() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_WRAP_R, &value);
    return GetWrapEnum(value);
}

TextureSampler::CompareMode TextureSampler::Impl::GetCompareMode() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_COMPARE_MODE, &value);
    return GetCompareModeEnum(value);
}

TextureSampler::CompareFunc TextureSampler::Impl::GetCompareFunc() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_COMPARE_FUNC, &value);
    return GetCompareFuncEnum(value);
}

float TextureSampler::Impl::GetMaxAnisotropy() const
{
    float value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_MAX_ANISOTROPY, &value);
    return value;
}

glm::vec4 TextureSampler::Impl::GetBorderColor() const
{
    glm::vec4 value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(value));
    return value;
}

void TextureSampler::Impl::SetMagFilter(Filter value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, GetGLEnum(value));
}

void TextureSampler::Impl::SetMinFilter(Filter value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, GetGLEnum(value));
}

void TextureSampler::Impl::SetMinLOD(float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_MIN_LOD, value);
}

void TextureSampler::Impl::SetMaxLOD(float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_MAX_LOD, value);
}

void TextureSampler::Impl::SetLODBias(float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_LOD_BIAS, value);
}

void TextureSampler::Impl::SetWrapS(Wrap value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, GetGLEnum(value));
}

void TextureSampler::Impl::SetWrapT(Wrap value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, GetGLEnum(value));
}

void TextureSampler::Impl::SetWrapR(Wrap value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_R, GetGLEnum(value));
}

void TextureSampler::Impl::SetCompareMode(CompareMode value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_MODE, GetGLEnum(value));
}

void TextureSampler::Impl::SetCompareFunc(CompareFunc value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GetGLEnum(value));
}

void TextureSampler::Impl::SetMaxAnisotropy(float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_MAX_ANISOTROPY, value);
}

void TextureSampler::Impl::SetBorderColor(glm::vec4 value)
{
    glSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(value));
}
