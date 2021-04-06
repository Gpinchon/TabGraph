/*
* @Author: gpinchon
* @Date:   2021-03-24 16:05:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-24 16:12:56
*/
#pragma once

#include "Renderer/SkyboxRenderer.hpp"

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <memory>

class TextureBuffer;
namespace Renderer {
    class SkyboxRenderer::Impl {
    public:
        void Render(Skybox& skybox, const ::Renderer::Options& options);

    private:
        std::array<GLsync, 2> _drawSync{ nullptr };
        std::array<std::shared_ptr<TextureBuffer>, 2> _jointMatrices{ nullptr };
        glm::mat4 _prevTransformMatrix{ 1 };
        int _jointMatricesIndex{ 0 };
    };
};