/*
* @Author: gpinchon
* @Date:   2021-04-10 15:33:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-12 21:59:40
*/

#include "Renderer/Light/LightRenderer.hpp"
#include "Light/Light.hpp"

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
};
