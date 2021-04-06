/*
* @Author: gpinchon
* @Date:   2021-03-22 20:48:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:50:13
*/

#pragma once

#include "Renderer/GeometryRenderer.hpp"
#include "Driver/OpenGL/VertexArray.hpp"

#include <memory>

//class VertexArray;

namespace Renderer {
    class GeometryRenderer::Impl {
    public:
        ~Impl();
        void OnFrameBegin(Geometry& geometry, uint32_t frameNbr, float delta);
        void Render(Geometry& geometry, bool doubleSided = false);
        void OnFrameEnd(Geometry& geometry, uint32_t frameNbr, float delta);

    private:
        std::unique_ptr<VertexArray> _vao;
    };
}
