/*
* @Author: gpinchon
* @Date:   2021-03-24 16:07:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-24 16:07:20
*/

//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/SkyboxRenderer.hpp"
//#endif
#include "Skybox.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Shader/Program.hpp"
#include "Texture/Cubemap.hpp"

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
};