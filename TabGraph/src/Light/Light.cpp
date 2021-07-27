/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:24
*/

#include <Light/Light.hpp>
#include <Texture/Texture.hpp>
#include <Renderer/Light/LightRenderer.hpp>

namespace TabGraph::Lights {
Light::Light()
    : Inherit()
{
    size_t s_lightNbr = 0u;
    SetName("Light_" + std::to_string(++s_lightNbr));
    ++s_lightNbr;
}

void Light::SetColor(const glm::vec3& color)
{
    if (GetColor() != color)
        GetRenderer().FlagDirty();
    _SetColor(color);
}

void Light::SetSpecularFactor(float factor)
{
    if (GetSpecularFactor() != factor)
        GetRenderer().FlagDirty();
    _SetSpecularFactor(factor);
}

void Light::SetDiffuseFactor(float factor)
{
    if (GetDiffuseFactor() != factor)
        GetRenderer().FlagDirty();
    _SetDiffuseFactor(factor);
}

Renderer::LightRenderer& Light::GetRenderer()
{
    return *_renderer;
}

}

void TabGraph::Renderer::LightRendererDeleter::operator()(Renderer::LightRenderer* ptr)
{
    delete ptr;
}