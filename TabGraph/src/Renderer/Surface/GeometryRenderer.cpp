/*
* @Author: gpinchon
* @Date:   2021-03-22 20:52:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-22 23:18:00
*/

#include "Renderer/Surface/SurfaceRenderer.hpp"

//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Surface/GeometryRenderer.hpp"
//#endif

#include "Surface/Geometry.hpp"

namespace Renderer {
void Render(std::shared_ptr<Geometry> geometry, bool doubleSided)
{
    static_cast<GeometryRenderer&>(geometry->GetRenderer()).Render(doubleSided);
}
};