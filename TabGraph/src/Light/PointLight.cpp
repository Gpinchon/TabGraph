/*
* @Author: gpinchon
* @Date:   2020-11-25 23:39:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 00:13:30
*/
#include <Light/PointLight.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Light/PointLightRenderer.hpp>
#endif
namespace TabGraph::Lights {
PointLight::PointLight(const std::string& name, glm::vec3 color, bool cast_shadow)
    : Inherit(name)
{
    _renderer.reset(new Renderer::PointLightRenderer(*this));
    SetColor(color);
    SetCastShadow(cast_shadow);
}
}
