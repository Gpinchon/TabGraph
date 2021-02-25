#include "Light/PointLight.hpp"
#include "Shader/Program.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Mesh/Geometry.hpp"
#include "Render.hpp"
#include "Framebuffer.hpp"

auto PointLightGeometry()
{
    static auto geometry = SphereMesh::CreateGeometry("PointLightGeometry", 1, 2);
    return geometry;
}

static inline auto PointLightVertexCode() {
    static auto deferred_vert_code =
#include "deferred.vert"
        ;
    static auto lightVertexCode =
#include "light.vert"
        ;
    static Shader::Stage::Code shaderCode = Shader::Stage::Code{ deferred_vert_code, "FillVertexData();" } + Shader::Stage::Code{ lightVertexCode, "TransformGeometry();" };
    return shaderCode;
}

static inline auto PointLightFragmentCode() {
    static auto deferred_frag_code =
#include "deferred.frag"
        ;
    static auto lightFragmentShader =
#include "pointLight.frag"
        ;
    static Shader::Stage::Code shaderCode = Shader::Stage::Code{ deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code{ lightFragmentShader, "Lighting();" };
    return shaderCode;
}

auto PointLightShader()
{
    static std::shared_ptr<Shader::Program> shader;
    if (shader == nullptr) {
        
        shader = Component::Create<Shader::Program>("PointLightShader");
        shader->SetDefine("LIGHTSHADER");
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
    auto geometryBuffer = Render::GeometryBuffer();
    PointLightShader()->Use();
    PointLightShader()->SetUniform("Light.Power", GetPower());
    PointLightShader()->SetUniform("Light.Radius", GetRadius());
    PointLightShader()->SetUniform("Light.Color", glm::value_ptr(GetColor()));
    PointLightShader()->SetUniform("Light.Position", glm::value_ptr(GetPosition()));
    PointLightShader()->SetUniform("Matrix.Model", glm::value_ptr(WorldTransformMatrix()));
    PointLightShader()->SetTexture("Texture.Geometry.F0", geometryBuffer->attachement(2));
    PointLightShader()->SetTexture("Texture.Geometry.Normal", geometryBuffer->attachement(4));
    PointLightShader()->SetTexture("Texture.Geometry.Depth", geometryBuffer->depth());
    PointLightGeometry()->Draw();
    PointLightShader()->Done();
}
