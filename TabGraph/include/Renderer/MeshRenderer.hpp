/*
* @Author: gpinchon
* @Date:   2021-03-22 20:32:34
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:54:56
*/
#pragma once

#include <glm/fwd.hpp>
#include <memory>

class Mesh;
namespace Renderer {
struct Options;
void OnFrameBegin(std::shared_ptr<Mesh> mesh, uint32_t frameNbr, float delta);
void Render(std::shared_ptr<Mesh> mesh, const ::Renderer::Options& options);
void Render(std::shared_ptr<Mesh> mesh, const ::Renderer::Options& options, const glm::mat4& parentTransform);
void Render(std::shared_ptr<Mesh> mesh, const ::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform);
void OnFrameEnd(std::shared_ptr<Mesh> mesh, uint32_t frameNbr, float delta);

class MeshRenderer {
    class Impl;
    friend Impl;

public:
    ~MeshRenderer();
    MeshRenderer(const MeshRenderer&) = delete;
    MeshRenderer(Mesh& mesh);
    Impl& GetImpl();
    void OnFrameBegin(uint32_t frameNbr, float delta);
    void Render(const ::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform);
    void OnFrameEnd(uint32_t frameNbr, float delta);

private:
    std::unique_ptr<Impl> _impl;
    Mesh& _mesh;
};
};