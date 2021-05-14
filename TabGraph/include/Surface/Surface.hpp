/*
* @Author: gpinchon
* @Date:   2021-05-12 14:12:25
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 14:14:19
*/
#pragma once

#include "Component.hpp"

namespace Renderer {
class SurfaceRenderer;
struct SurfaceRendererDeleter
{
    void operator()(SurfaceRenderer* p);
};
}

class Surface : public Component {
public:
    Surface(const std::string& name) : Component(name) {}
    Surface(const Surface& other) : Component(other) {}
    ~Surface();
    Renderer::SurfaceRenderer& GetRenderer() const;

protected:
    std::unique_ptr<Renderer::SurfaceRenderer, Renderer::SurfaceRendererDeleter> _renderer;
};
