/*
* @Author: gpinchon
* @Date:   2021-03-22 20:52:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-22 23:18:00
*/

#include <Renderer/Surface/ShapeRenderer.hpp>
#include <Surface/Geometry.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Surface/GeometryRenderer.hpp>
#endif



namespace Renderer {
void Render(std::shared_ptr<Geometry> geometry, bool doubleSided)
{
    static_cast<GeometryRenderer&>(geometry->GetRenderer()).Render(doubleSided);
}
};