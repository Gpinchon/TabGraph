/*
* @Author: gpinchon
* @Date:   2021-03-23 13:41:11
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:49:54
*/
#pragma once

#include "Renderer/MeshRenderer.hpp"

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <memory>

class TextureBuffer;

namespace Renderer {
class MeshRenderer::Impl {
public:
    void Load(Mesh& mesh);
    void OnFrameBegin(Mesh& mesh, uint32_t frameNbr, float delta);
    void Render(Mesh& mesh, const ::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform);
    void OnFrameEnd(Mesh& mesh, uint32_t frameNbr, float delta);

private:
    std::array<GLsync, 2> _drawSync{ nullptr };
    std::array<std::shared_ptr<TextureBuffer>, 2> _jointMatrices{ nullptr };
    glm::mat4 _prevTransformMatrix{ 1 };
    int _jointMatricesIndex{ 0 };
    bool _loaded{ false };
};
};
