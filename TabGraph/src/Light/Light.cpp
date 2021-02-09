/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:24
*/

#include "Light/Light.hpp"
#include "Texture/Texture.hpp" // for Texture

auto g_lightNbr = 0u;

Light::Light()
    : Node("Light_" + std::to_string(g_lightNbr))
{
    ++g_lightNbr;
}

Light::Light(const std::string& name, glm::vec3 color)
    : Light()
{
    SetColor(color);
}
