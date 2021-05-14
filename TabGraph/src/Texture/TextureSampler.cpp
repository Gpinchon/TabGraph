/*
* @Author: gpinchon
* @Date:   2021-04-29 18:57:43
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-01 23:56:53
*/
#include "Texture/TextureSampler.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Texture/TextureSampler.hpp"
//#endif

#include <array>

TextureSampler::TextureSampler()
    : _impl(new TextureSampler::Impl)
{
}

TextureSampler::~TextureSampler()
{
}

TextureSampler::Impl& TextureSampler::GetImpl()
{
    return *_impl.get();
}

TextureSampler::Filter TextureSampler::GetMagFilter() const
{
    return _impl->GetMagFilter();
}

TextureSampler::Filter TextureSampler::GetMinFilter() const
{
    return _impl->GetMinFilter();
}

float TextureSampler::GetMinLOD() const
{
    return _impl->GetMinLOD();
}

float TextureSampler::GetMaxLOD() const
{
    return _impl->GetMaxLOD();
}

float TextureSampler::GetLODBias() const
{
    return _impl->GetLODBias();
}

TextureSampler::Wrap TextureSampler::GetWrapS() const
{
    return _impl->GetWrapS();
}

TextureSampler::Wrap TextureSampler::GetWrapT() const
{
    return _impl->GetWrapT();
}

TextureSampler::Wrap TextureSampler::GetWrapR() const
{
    return _impl->GetWrapR();
}

TextureSampler::CompareMode TextureSampler::GetCompareMode() const
{
    return _impl->GetCompareMode();
}

TextureSampler::CompareFunc TextureSampler::GetCompareFunc() const
{
    return _impl->GetCompareFunc();
}

float TextureSampler::GetMaxAnisotropy() const
{
    return _impl->GetMaxAnisotropy();
}

glm::vec4 TextureSampler::GetBorderColor() const
{
    return _impl->GetBorderColor();
}

void TextureSampler::SetMagFilter(Filter value)
{
    _impl->SetMagFilter(value);
}

void TextureSampler::SetMinFilter(Filter value)
{
    _impl->SetMinFilter(value);
}

void TextureSampler::SetMinLOD(float value)
{
    _impl->SetMinLOD(value);
}

void TextureSampler::SetMaxLOD(float value)
{
    _impl->SetMaxLOD(value);
}

void TextureSampler::SetLODBias(float value)
{
    _impl->SetLODBias(value);
}

void TextureSampler::SetWrapS(Wrap value)
{
    _impl->SetWrapS(value);
}

void TextureSampler::SetWrapT(Wrap value)
{
    _impl->SetWrapT(value);
}

void TextureSampler::SetWrapR(Wrap value)
{
    _impl->SetWrapR(value);
}

void TextureSampler::SetCompareMode(CompareMode value)
{
    _impl->SetCompareMode(value);
}

void TextureSampler::SetCompareFunc(CompareFunc value)
{
    _impl->SetCompareFunc(value);
}

void TextureSampler::SetMaxAnisotropy(float value)
{
    _impl->SetMaxAnisotropy(value);
}

void TextureSampler::SetBorderColor(glm::vec4 value)
{
    _impl->SetBorderColor(value);
}
