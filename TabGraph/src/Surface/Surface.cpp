/*
* @Author: gpinchon
* @Date:   2021-05-12 14:28:31
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 14:29:01
*/

#include "Surface/Surface.hpp"
#include "Renderer/Surface/SurfaceRenderer.hpp"

Surface::~Surface()
{
}

Renderer::SurfaceRenderer& Surface::GetRenderer() const {
    return *_renderer.get();
}

void Renderer::SurfaceRendererDeleter::operator()(SurfaceRenderer* p)
{
    delete p;
}
