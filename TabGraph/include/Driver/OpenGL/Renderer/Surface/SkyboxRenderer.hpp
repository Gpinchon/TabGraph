/*
* @Author: gpinchon
* @Date:   2021-03-24 16:05:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-24 16:12:56
*/
#pragma once

#include "Renderer/Surface/SurfaceRenderer.hpp"

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <memory>

class Skybox;
class TextureBuffer;
namespace Shader {
class Program;
};

namespace Renderer {
struct Options;
class SkyboxRenderer : public SurfaceRenderer {
public:
    SkyboxRenderer(Skybox&);
    SkyboxRenderer(SkyboxRenderer&) = delete;
    void Render(const Options& options);
    virtual void OnFrameBegin(uint32_t frameNbr, float delta) override;
    virtual void Render(const::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform) override;
    virtual void OnFrameEnd(uint32_t frameNbr, float delta) override;

private:
    std::shared_ptr<Shader::Program> _shader;
    std::array<GLsync, 2> _drawSync{ nullptr };
    std::array<std::shared_ptr<TextureBuffer>, 2> _jointMatrices{ nullptr };
    glm::mat4 _prevTransformMatrix{ 1 };
    int _jointMatricesIndex{ 0 };
    Skybox& _skybox;    
};
};