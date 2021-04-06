/*
* @Author: gpinchon
* @Date:   2021-03-24 16:05:51
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:49:42
*/
#pragma once

#include <glm/fwd.hpp>
#include <memory>

class Skybox;

namespace Renderer {
struct Options;
void OnFrameBegin(std::shared_ptr<Skybox> skybox, uint32_t frameNbr, float delta);
void Render(std::shared_ptr<Skybox> skybox, const Renderer::Options& options);
void OnFrameEnd(std::shared_ptr<Skybox> skybox, uint32_t frameNbr, float delta);

class SkyboxRenderer {
    class Impl;
    friend Impl;

public:
    ~SkyboxRenderer();
    SkyboxRenderer(const SkyboxRenderer&) = delete;
    SkyboxRenderer(Skybox& skybox);
    Impl& GetImpl();
    void Render(const Renderer::Options& options);

private:
    std::unique_ptr<Impl> _impl;
    Skybox& _skybox;
};
};