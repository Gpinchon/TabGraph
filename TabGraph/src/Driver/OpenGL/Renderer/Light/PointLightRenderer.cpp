/*
* @Author: gpinchon
* @Date:   2021-04-11 20:53:00
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 20:53:22
*/

#include <Driver/OpenGL/Renderer/Light/PointLightRenderer.hpp>
#include <Light/PointLight.hpp>
#include <Surface/SphereMesh.hpp>
#include <Renderer/Surface/GeometryRenderer.hpp>
#include <Renderer/Renderer.hpp>
#include <Shader/Program.hpp>
#include <Shader/Stage.hpp>
#include <Texture/Framebuffer.hpp>
#include <Texture/Texture2D.hpp>

#include <GL/glew.h>

namespace Renderer {

static inline auto PointLightGeometry()
{
    static std::weak_ptr<Geometry> s_geometry;
    auto geometryPtr = s_geometry.lock();
    if (geometryPtr == nullptr) {
        geometryPtr = SphereMesh::CreateGeometry("PointLightGeometry", 1, 1);
        s_geometry = geometryPtr;
    }
    return geometryPtr;
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
    auto shader = Component::Create<Shader::Program>("PointLightShader");
    shader->SetDefine("Pass", "DeferredLighting");
    shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, PointLightFragmentCode()));
    shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, PointLightVertexCode()));
    return shader;
}

PointLightRenderer::PointLightRenderer(PointLight &light)
    : LightRenderer(light)
{
    _deferredShader = PointLightShader();
    _deferredGeometry = PointLightGeometry();
}

void PointLightRenderer::Render(const Renderer::Options& options)
{
    if (options.pass == Renderer::Options::Pass::DeferredLighting)
        _RenderDeferredLighting(static_cast<PointLight&>(_light), options);
    else if (options.pass == Renderer::Options::Pass::ShadowDepth)
        _RenderShadow(static_cast<PointLight&>(_light), options);
}

void PointLightRenderer::UpdateLightProbe(const Renderer::Options&, LightProbe&)
{
}

void PointLightRenderer::_RenderDeferredLighting(PointLight& light, const Renderer::Options& options)
{
    auto geometryBuffer = options.renderer->DeferredGeometryBuffer();
    _deferredShader->Use()
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
    glCullFace(GL_FRONT);
    Renderer::Render(_deferredGeometry);
    glCullFace(GL_BACK);
    _deferredShader->Done();
}

void PointLightRenderer::_RenderShadow(PointLight&, const Renderer::Options&)
{
}
}