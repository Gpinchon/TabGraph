/*
* @Author: gpinchon
* @Date:   2021-03-25 11:04:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:55:36
*/
#pragma once

#include <glm/fwd.hpp>
#include <memory>

class Scene;
class LightProbe;

namespace Renderer {
struct Options;
void OnFrameBegin(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta);
void Render(std::shared_ptr<Scene> scene, const Renderer::Options& options);
void Render(std::shared_ptr<Scene> scene, const Renderer::Options& options, const glm::mat4& rootMatrix);
void OnFrameEnd(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta);

class SceneRenderer {
    class Impl;
    friend Impl;

public:
    SceneRenderer(Scene& scene);
    SceneRenderer(const SceneRenderer&) = delete;
    ~SceneRenderer();
    Impl& GetImpl();
    LightProbe& GetClosestLightProbe(const glm::vec3 &position);
    void OnFrameBegin(uint32_t frameNbr, float delta);
    void Render(const ::Renderer::Options& options);
    void Render(const ::Renderer::Options& options, const glm::mat4& rootMatrix);
    void OnFrameEnd(uint32_t frameNbr, float delta);

private:
    std::unique_ptr<Impl> _impl;
    Scene& _scene;
};
};