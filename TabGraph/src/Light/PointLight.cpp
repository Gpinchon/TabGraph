/*
* @Author: gpinchon
* @Date:   2020-11-25 23:39:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-14 20:19:56
*/
#include "Light/PointLight.hpp"
#include "Framebuffer.hpp"
#include "Texture/Texture2D.hpp"
#include "Mesh/Geometry.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Shader/Program.hpp"

static inline auto PointLightGeometry()
{
    static auto geometry = SphereMesh::CreateGeometry("PointLightGeometry", 1, 1);
    return geometry;
}

static inline auto PointLightVertexCode()
{
    static auto deferred_vert_code =
#include "deferred.vert"
        ;
    static auto lightVertexCode =
#include "Lights/TransformGeometry.vert"
        ;
    static Shader::Stage::Code shaderCode = Shader::Stage::Code { lightVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto PointLightFragmentCode()
{
    static auto deferred_frag_code =
#include "deferred.frag"
        ;
    static auto lightFragmentShader =
#include "Lights/DeferredPointLight.frag"
        ;
    static Shader::Stage::Code shaderCode =
        Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } +
        Shader::Stage::Code { lightFragmentShader, "Lighting();" };
    return shaderCode;
}

static inline auto PointLightShader()
{
    static std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {

        shader = Component::Create<Shader::Program>("PointLightShader");
        shader->SetDefine("Pass", "DeferredLighting");
        shader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, PointLightFragmentCode()));
        shader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, PointLightVertexCode()));
    }
    return shader;
}

PointLight::PointLight(const std::string& name, glm::vec3 color, bool cast_shadow)
{
    SetName(name);
    SetColor(color);
    SetCastShadow(cast_shadow);
}

void PointLight::render_shadow()
{
}

void PointLight::Draw()
{
    SetScale(glm::vec3(GetRadius()));
    auto geometryBuffer = Renderer::DeferredGeometryBuffer();
    PointLightShader()->Use()
        .SetUniform("Light.Power", GetPower())
        .SetUniform("Light.Radius", GetRadius())
        .SetUniform("Light.Color", GetColor())
        .SetUniform("Light.Position", WorldPosition())
        .SetUniform("GeometryMatrix", WorldTransformMatrix())
        .SetTexture("Texture.Geometry.F0", geometryBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", geometryBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", geometryBuffer->GetDepthBuffer());
    Renderer::Render(PointLightGeometry());
    PointLightShader()->Done();
}

void PointLight::DrawProbe(LightProbe& lightProbe)
{
}
