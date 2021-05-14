/*
* @Author: gpinchon
* @Date:   2021-05-12 13:38:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 15:59:18
*/
#pragma once

#include <glm/fwd.hpp>
#include <memory>

class Surface;

namespace Renderer {
struct Options;
struct SurfaceRenderer {
    virtual void OnFrameBegin(uint32_t frameNbr, float delta) = 0;
    virtual void Render(const ::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform) = 0;
    virtual void OnFrameEnd(uint32_t frameNbr, float delta) = 0;
};
void OnFrameBegin(std::shared_ptr<Surface> surface, uint32_t frameNbr, float delta);
void Render(std::shared_ptr<Surface> surface, const Options& options);
void Render(std::shared_ptr<Surface> surface, const Options& options, const glm::mat4& parentTransform);
void Render(std::shared_ptr<Surface> surface, const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform);
void OnFrameEnd(std::shared_ptr<Surface> surface, uint32_t frameNbr, float delta);
};