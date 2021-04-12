/*
* @Author: gpinchon
* @Date:   2021-03-23 13:41:11
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:49:54
*/
#pragma once

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <memory>

class TextureBuffer;
class Mesh;

namespace Renderer {
struct Options;
class MeshRenderer {
public:
    MeshRenderer(Mesh&);
    MeshRenderer(const MeshRenderer&) = delete;
    void Load();
    void OnFrameBegin(uint32_t frameNbr, float delta);
    void Render(const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform);
    void OnFrameEnd(uint32_t frameNbr, float delta);

private:
    std::array<GLsync, 2> _drawSync{ nullptr };
    std::array<std::shared_ptr<TextureBuffer>, 2> _jointMatrices{ nullptr };
    glm::mat4 _prevTransformMatrix{ 1 };
    int _jointMatricesIndex{ 0 };
    bool _loaded{ false };
    Mesh& _mesh;
};
};
