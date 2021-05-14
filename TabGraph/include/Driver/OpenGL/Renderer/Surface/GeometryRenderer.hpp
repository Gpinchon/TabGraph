/*
* @Author: gpinchon
* @Date:   2021-03-22 20:48:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:50:13
*/

#pragma once

#include "Renderer/Surface/SurfaceRenderer.hpp"

#include <memory>

class Geometry;
class VertexArray;

namespace Renderer {
class GeometryRenderer : public SurfaceRenderer {
public:
    GeometryRenderer(Geometry& geometry);
    GeometryRenderer(const GeometryRenderer&) = delete;
    ~GeometryRenderer();
    void OnFrameBegin(uint32_t frameNbr, float delta);
    void Render(bool doubleSided = false);
    virtual void Render(const::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform) override;
    void OnFrameEnd(uint32_t frameNbr, float delta);

private:
    Geometry& _geometry;
    std::unique_ptr<VertexArray> _vao;
};
}
