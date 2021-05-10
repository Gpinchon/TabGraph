/*
* @Author: gpinchon
* @Date:   2021-04-11 20:53:00
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 20:53:22
*/

#include "Driver/OpenGL/Renderer/Light/PointLightRenderer.hpp"
#include "Light/PointLight.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Shader/Program.hpp"
#include "Shader/Stage.hpp"
#include "Framebuffer.hpp"
#include "Texture/Texture2D.hpp"

namespace Renderer {
static inline auto PointLightGeometry()
{
    static auto geometry = SphereMesh::CreateGeometry("PointLightGeometry", 1, 1);
    return geometry;
}

static inline auto PointLightVertexCode()
{
    auto deferred_vert_code =
#include "deferred.vert"
        ;
    auto lightVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    Shader::Stage::Code shaderCode = Shader::Stage::Code{ lightVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto PointLightFragmentCode()
{
    auto deferred_frag_code =
#include "deferred.frag"
        ;
    auto lightFragmentShader =
#include "Lights/DeferredPointLight.frag"
        ;
    Shader::Stage::Code shaderCode =
        Shader::Stage::Code{ deferred_frag_code, "FillFragmentData();" } +
        Shader::Stage::Code{ lightFragmentShader, "Lighting();" };
    return shaderCode;
}

static inline auto PointLightShader()
{
    std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {

        shader = Component::Create<Shader::Program>("PointLightShader");
        shader->SetDefine("Pass", "DeferredLighting");
        shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, PointLightFragmentCode()));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, PointLightVertexCode()));
    }
    return shader;
}

PointLightRenderer::PointLightRenderer(PointLight &light)
    : LightRenderer(light)
{
    _lightingShader = PointLightShader();
}

void PointLightRenderer::Render(const Renderer::Options& options)
{
    if (options.pass == Renderer::Options::Pass::DeferredLighting)
        _RenderDeferredLighting(static_cast<PointLight&>(_light), options);
    else if (options.pass == Renderer::Options::Pass::ShadowDepth)
        _RenderShadow(static_cast<PointLight&>(_light), options);
}

void PointLightRenderer::UpdateLightProbe(LightProbe&)
{
}

void PointLightRenderer::_RenderDeferredLighting(PointLight& light, const Renderer::Options& options)
{
    auto geometryBuffer = Renderer::DeferredGeometryBuffer();
    _lightingShader->Use()
        .SetUniform("Light.DiffuseFactor", light.GetDiffuseFactor())
        .SetUniform("Light.SpecularFactor", light.GetSpecularFactor())
        .SetUniform("Light.Power", light.GetPower())
        .SetUniform("Light.Radius", light.GetRadius())
        .SetUniform("Light.Color", light.GetColor())
        .SetUniform("Light.Position", light.WorldPosition())
        .SetUniform("GeometryMatrix", light.WorldTranslationMatrix() * glm::scale(glm::vec3(light.GetRadius())))
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    Renderer::Render(PointLightGeometry());
    _lightingShader->Done();
}

void PointLightRenderer::_RenderShadow(PointLight&, const Renderer::Options&)
{
}
}