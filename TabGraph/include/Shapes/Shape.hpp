/*
* @Author: gpinchon
* @Date:   2021-06-29 22:46:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:44
*/
#pragma once

#include <Core/Inherit.hpp>
#include <Core/Object.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace Renderer {
class ShapeRenderer;
struct ShapeRendererDeleter {
    void operator()(ShapeRenderer* p);
};
};

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Shape : public Core::Inherit<Core::Object, Shape> {
public:
    ~Shape();
    Renderer::ShapeRenderer& GetRenderer()
    {
        return *_renderer;
    }

protected:
    std::unique_ptr<Renderer::ShapeRenderer, Renderer::ShapeRendererDeleter> _renderer;
};
};
