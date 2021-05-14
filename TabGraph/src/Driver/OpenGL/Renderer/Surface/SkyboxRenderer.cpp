/*
* @Author: gpinchon
* @Date:   2021-03-24 16:07:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/

#include "Driver/OpenGL/Renderer/Surface/SkyboxRenderer.hpp"
#include "Renderer/Surface/GeometryRenderer.hpp"
#include "Renderer/Renderer.hpp"
#include "Shader/Program.hpp"
#include "Surface/Skybox.hpp"
#include "Texture/TextureCubemap.hpp"

static inline auto EnvShader()
{
    auto deferredVertCode =
#include "deferred.vert"
        ;
    auto skyboxFragCode =
#include "Skybox.frag"
        ;
    std::shared_ptr<Shader::Program> shader = Component::Create<Shader::Program>("EnvShader");
    shader->SetDefine("Pass", "DeferredLighting");
    shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { deferredVertCode, "FillVertexData();" }));
    shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { skyboxFragCode, "OutputEnv();" }));
    return shader;
}

namespace Renderer {
SkyboxRenderer::SkyboxRenderer(Skybox& skybox)
    : _skybox(skybox)
{
    _shader = EnvShader();
}

void SkyboxRenderer::Render(const Renderer::Options& options)
{
    _shader->Use().SetTexture("Skybox", _skybox.GetTexture());
    Renderer::Render(Renderer::DisplayQuad(), true);
    _shader->Done();
}

void SkyboxRenderer::OnFrameBegin(uint32_t frameNbr, float delta)
{
}

void SkyboxRenderer::Render(const::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform)
{
    Render(options);
}

void SkyboxRenderer::OnFrameEnd(uint32_t frameNbr, float delta)
{
}
};