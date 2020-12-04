#include "Light\PointLight.hpp"

#include "Shader/Shader.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Mesh/Geometry.hpp"
#include "Render.hpp"
#include "Framebuffer.hpp"

auto PointLightGeometry()
{
    static auto geometry = SphereMesh::CreateGeometry("PointLightGeometry", 1, 2);
    return geometry;
}

auto PointLightShader()
{
    static std::shared_ptr<Shader> shader;
    if (shader == nullptr) {
        auto lightingFragmentCode =
#include "pointLight.frag"
            ;
        auto lightingVertexCode =
#include "light.vert"
            ;
        shader = Component::Create<Shader>("PointLightShader", Shader::Type::LightingShader);
        shader->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(lightingVertexCode, "TransformGeometry();")));
        shader->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(lightingFragmentCode, "Lighting();"));
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
    //PointLightShader()->SetUniform("Light.Projection", Infinite() ? ShadowProjectionMatrixInfinite() : ShadowProjectionMatrixFinite());
    /*float constant = 1.0;
    float linear = 0.7;
    float quadratic = 1.8;
    float lightMax = std::fmaxf(std::fmaxf(GetColor().r, GetColor().g), GetColor().b);
    float radius =
        (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax)))
        / (2 * quadratic);*/
    SetScale(glm::vec3(GetRadius()));
    auto geometryBuffer = Render::GeometryBuffer();
    PointLightShader()->SetUniform("Light.Cutoff", GetCutoff());
    PointLightShader()->SetUniform("Light.Radius", GetRadius());
    PointLightShader()->SetUniform("Light.Color", GetColor());
    PointLightShader()->SetUniform("Light.Position", GetPosition());
    PointLightShader()->SetUniform("Matrix.Model", WorldTransformMatrix());
    PointLightShader()->SetTexture("Texture.Geometry.F0", geometryBuffer->attachement(2));
    PointLightShader()->SetTexture("Texture.Geometry.Normal", geometryBuffer->attachement(4));
    PointLightShader()->SetTexture("Texture.Geometry.Depth", geometryBuffer->depth());
    PointLightShader()->use();
    PointLightGeometry()->Draw();
    PointLightShader()->use(false);
}
