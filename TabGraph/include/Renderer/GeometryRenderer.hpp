/*
* @Author: gpinchon
* @Date:   2021-03-22 20:42:01
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:54:53
*/

#pragma once

#include <memory>

class Geometry;
class BufferAccessor;

namespace Renderer {
class Options;
void OnFrameBegin(std::shared_ptr<Geometry> geometry, uint32_t frameNbr, float delta);
void Render(std::shared_ptr<Geometry> geometry, bool doubleSided = false);
void OnFrameEnd(std::shared_ptr<Geometry> geometry, uint32_t frameNbr, float delta);

class GeometryRenderer {
    class Impl;
    friend Impl;

public:
    GeometryRenderer(const GeometryRenderer&) = delete;
    GeometryRenderer(Geometry& geometry);
    ~GeometryRenderer();
    Impl& GetImpl();
    void OnFrameBegin(uint32_t frameNbr, float delta);
    void Render(bool doubleSided = false);
    void OnFrameEnd(uint32_t frameNbr, float delta);

private:
    std::unique_ptr<Impl> _impl;
    Geometry& _geometry;
};
};