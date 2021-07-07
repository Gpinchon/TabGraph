/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:30:50
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Nodes/Node.hpp> // for Node
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

#include <glm/glm.hpp> // for glm::vec3
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
class LightRenderer;
struct LightRendererDeleter {
    void operator()(Renderer::LightRenderer*);
};
namespace Lights {
class Probe;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Lights {
class Light : public Core::Inherit<Nodes::Node, Light> {
    PROPERTY(bool, CastShadow, false);
    READONLYPROPERTY(glm::vec3, Color, 1);
    READONLYPROPERTY(float, SpecularFactor, 1);
    READONLYPROPERTY(float, DiffuseFactor, 1);

public:
    Light(const std::string& name, glm::vec3 color);
    Light();
    void SetColor(const glm::vec3& color);
    void SetSpecularFactor(float factor);
    void SetDiffuseFactor(float factor);
    Renderer::LightRenderer& GetRenderer();

protected:
    std::unique_ptr<Renderer::LightRenderer, Renderer::LightRendererDeleter> _renderer;
};
}
