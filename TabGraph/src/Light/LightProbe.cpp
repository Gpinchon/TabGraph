/*
* @Author: gpinchon
* @Date:   2021-03-18 01:33:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:23
*/

#include "Light/LightProbe.hpp"
#include "Framebuffer.hpp"
#include "Texture/TextureCubemap.hpp"
#include "Texture/TextureSampler.hpp"

LightProbe::LightProbe(LightProbeGroup& lightProbeGroup)
    : _lightProbeGroup(lightProbeGroup)
{
    SetReflectionBuffer(Component::Create<Framebuffer>("DiffuseLUTBuffer", lightProbeGroup.GetResolution()));
    auto cubemap { Component::Create<TextureCubemap>(lightProbeGroup.GetResolution(), Pixel::SizedFormat::Float16_RGB) };
    cubemap->GetTextureSampler()->SetMinFilter(TextureSampler::Filter::LinearMipmapLinear);
    GetReflectionBuffer()->AddColorBuffer(cubemap);
}

const SphericalHarmonics& LightProbe::GetSphericalHarmonics() const
{
    return _lightProbeGroup._sphericalHarmonics;
}

glm::vec3 LightProbe::GetAbsolutePosition() const
{
    return _lightProbeGroup.GetPosition() + GetPosition();
}

std::vector<LightProbe>& LightProbeGroup::GetLightProbes()
{
    return _lightProbes;
}
