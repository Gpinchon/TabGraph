/*
* @Author: gpinchon
* @Date:   2020-11-25 23:39:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-14 20:19:56
*/
#include "Light/PointLight.hpp"
#include "Renderer/Light/PointLightRenderer.hpp"

PointLight::PointLight(const std::string& name, glm::vec3 color, bool cast_shadow)
    : Light()
{
    _renderer.reset(new Renderer::PointLightRenderer(*this));
    SetName(name);
    SetColor(color);
    SetCastShadow(cast_shadow);
}
