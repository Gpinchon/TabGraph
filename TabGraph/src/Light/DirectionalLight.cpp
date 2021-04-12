/*
* @Author: gpinchon
* @Date:   2020-11-24 21:47:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-15 00:04:45
*/

#include "Light/DirectionalLight.hpp"
#include "Renderer/Light/DirectionalLightRenderer.hpp"

#include <glm/glm.hpp>

DirectionalLight::DirectionalLight()
    : Light()
{
    static auto g_dirLightNbr = 0u;
    SetName("DirectionalLight_" + std::to_string(g_dirLightNbr));
    ++g_dirLightNbr;
    _renderer.reset(new Renderer::DirectionalLightRenderer(*this));
}

DirectionalLight::DirectionalLight(const std::string& name, glm::vec3 color, glm::vec3 direction, bool cast_shadow)
    : DirectionalLight()
{
    SetName(name);
    SetColor(color);
    SetDirection(direction);
    SetHalfSize(glm::vec3(500));
    SetCastShadow(cast_shadow);
}

void DirectionalLight::SetDirection(const glm::vec3& direction)
{
    _SetDirection(normalize(direction));
}

glm::vec3 DirectionalLight::GetHalfSize() const
{
    return GetScale() / 2.f;
}

void DirectionalLight::SetHalfSize(const glm::vec3& halfSize)
{
    SetScale(halfSize * 2.f);
}

glm::vec3 DirectionalLight::GetMin() const
{
    return WorldPosition() - GetHalfSize();
}

glm::vec3 DirectionalLight::GetMax() const
{
    return WorldPosition() + GetHalfSize();
}
