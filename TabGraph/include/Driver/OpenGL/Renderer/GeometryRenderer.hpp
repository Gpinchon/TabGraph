/*
* @Author: gpinchon
* @Date:   2021-03-22 20:48:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:50:13
*/

#pragma once

#include "Renderer/GeometryRenderer.hpp"

#include <memory>

class Geometry;
class VertexArray;

namespace Renderer {
    class GeometryRenderer {
    public:
        GeometryRenderer(Geometry& geometry);
        GeometryRenderer(const GeometryRenderer&) = delete;
        ~GeometryRenderer();
        void OnFrameBegin(uint32_t frameNbr, float delta);
        void Render(bool doubleSided = false);
        void OnFrameEnd(uint32_t frameNbr, float delta);

    private:
        Geometry& _geometry;
        std::unique_ptr<VertexArray> _vao;
    };
}
