/*
* @Author: gpinchon
* @Date:   2021-03-22 20:52:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-22 23:18:00
*/

#include "Renderer/GeometryRenderer.hpp"

#include "Mesh/Geometry.hpp"

namespace Renderer {
    void OnFrameBegin(std::shared_ptr<Geometry> geometry, uint32_t frameNbr, float delta)
    {
        geometry->GetRenderer().OnFrameBegin(frameNbr, delta);
    }
    void Render(std::shared_ptr<Geometry> geometry, bool doubleSided)
    {
        geometry->GetRenderer().Render(doubleSided);
    }
    void OnFrameEnd(std::shared_ptr<Geometry> geometry, uint32_t frameNbr, float delta)
    {
        geometry->GetRenderer().OnFrameEnd(frameNbr, delta);
    }
    /*GeometryRenderer::GeometryRenderer(Geometry& geometry)
        : _impl(new GeometryRenderer::Impl())
        , _geometry(geometry)
    {
    }

    GeometryRenderer::~GeometryRenderer()
    {
    }

    GeometryRenderer::Impl& GeometryRenderer::GetImpl()
    {
        return *_impl;
    }

    void GeometryRenderer::OnFrameBegin(uint32_t frameNbr, float delta)
    {
        GetImpl().OnFrameBegin(_geometry, frameNbr, delta);
    }

    void GeometryRenderer::Render(bool doubleSided)
    {
        GetImpl().Render(_geometry, doubleSided);
    }

    void GeometryRenderer::OnFrameEnd(uint32_t frameNbr, float delta)
    {
        GetImpl().OnFrameEnd(_geometry, frameNbr, delta);
    }*/
};