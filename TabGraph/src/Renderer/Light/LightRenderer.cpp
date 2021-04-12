/*
* @Author: gpinchon
* @Date:   2021-04-10 15:33:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 14:53:15
*/

#include "Renderer/Light/LightRenderer.hpp"
#include "Light/Light.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/LightRenderer.hpp"
//#endif

namespace Renderer {
void Render(std::shared_ptr<Light> light, const Renderer::Options& options)
{
    light->GetRenderer().Render(options);
}
void UpdateLightProbe(std::shared_ptr<Light> light, LightProbe& lightProbe)
{
    light->GetRenderer().UpdateLightProbe(lightProbe);
}
LightRenderer::LightRenderer(Light& light)
    : _light(light)
{
}
void LightRenderer::Render(const Renderer::Options& options)
{
    GetImpl().Render(_light, options);
}
void LightRenderer::UpdateLightProbe(LightProbe& lightProbe)
{
    GetImpl().UpdateLightProbe(_light, lightProbe);
}
LightRenderer::Impl& LightRenderer::GetImpl()
{
    return *_impl;
}
void LightRenderer::ImplDeleter::operator()(LightRenderer::Impl* ptr)
{
    delete ptr;
}
};
