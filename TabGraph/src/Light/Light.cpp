/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:45:36
*/

#include "Light/Light.hpp"
#include "Texture/Texture.hpp" // for Texture
#include "Transform.hpp"

auto g_lightNbr = 0u;

Light::Light() : Node("Light_" + std::to_string(g_lightNbr))
{
    ++g_lightNbr;
}

Light::Light(const std::string& name, glm::vec3 color, glm::vec3 position, float power)
    : Light()
{
    SetPosition(position);
    SetColor(color);
    SetPower(power);
}

glm::vec3 Light::Color() const
{
    return _color;
}

void Light::SetPower(const float& power)
{
    _power = power;
}

float Light::Power() const
{
    return _power;
}

void Light::SetCastShadow(bool castShadow)
{
    _cast_shadow = castShadow;
}

bool Light::CastShadow() const
{
    return (_cast_shadow);
}

void Light::SetColor(const glm::vec3& color)
{
    _color = color;
}
