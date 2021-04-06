/*
* @Author: gpinchon
* @Date:   2021-03-24 16:07:09
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-24 16:07:09
*/

#include "Renderer/SkyboxRenderer.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/SkyboxRenderer.hpp"
//#endif
#include "Skybox.hpp"

namespace Renderer {
    void SkyboxRenderer::Render(const::Renderer::Options& options)
    {
        GetImpl().Render(_skybox, options);
    }

    void Render(std::shared_ptr<Skybox> skybox, const Renderer::Options& options)
    {
        skybox->GetRenderer().Render(options);
    }
};