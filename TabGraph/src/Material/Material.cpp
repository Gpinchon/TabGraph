/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-31 17:28:44
*/

#include "Material/Material.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Material/MaterialExtension.hpp"
#include "Scene/Scene.hpp" // for Scene
#include "Shader/Program.hpp" // for Shader::Program
#include "Shader/Stage.hpp" // for Shader::Stage
#include "Texture/Cubemap.hpp"
#include "Texture/Texture2D.hpp"
//#include "brdfLUT.hpp"

static inline auto GetSHExtension()
{
    auto shaderCode =
#include "sphericalHarmonics.glsl"
        ;
    return Shader::Stage::Code(shaderCode);
}

static inline auto GetCheckOpacityExtension()
{
    auto checkOpacity =
#include "checkOpacity.glsl"
        ;
    return Shader::Stage::Code(checkOpacity, "CheckOpacity();");
}

static inline auto GetForwardLitPassExtension()
{
    auto forwardLitPass =
#include "forwardLit.frag"
        ;
    return Shader::Stage::Code(forwardLitPass, "ComputeColor();");
}

static inline auto GetForwardVertexExtension()
{
    auto forward_vert_code =
#include "forward.vert"
        ;
    return Shader::Stage::Code(forward_vert_code, "FillVertexData();");
}

static inline auto GetForwardFragmentExtension()
{
    auto forward_frag_code =
#include "forward.frag"
        ;
    return Shader::Stage::Code(forward_frag_code, "FillFragmentData();");
}

Material::Material(const std::string& name)
    : MaterialExtension(name)
{
    SetShader(Renderer::Options::Pass::DeferredGeometry, Component::Create<Shader::Program>(GetName() + "_geometryShader"));
    GetShader(Renderer::Options::Pass::DeferredGeometry)->SetDefine("Pass", "DeferredGeometry");
    GetShader(Renderer::Options::Pass::DeferredGeometry)->Attach(Shader::Stage(Shader::Stage::Type::Vertex,
        GetForwardVertexExtension()
    ));
    GetShader(Renderer::Options::Pass::DeferredGeometry)->Attach(Shader::Stage(Shader::Stage::Type::Fragment,
        GetForwardFragmentExtension() +
        GetCheckOpacityExtension()
    ));

    SetShader(Renderer::Options::Pass::ForwardTransparent, Component::Create<Shader::Program>(GetName() + "_materialShader"));
    GetShader(Renderer::Options::Pass::ForwardTransparent)->SetDefine("Pass", "ForwardTransparent");
    GetShader(Renderer::Options::Pass::ForwardTransparent)->Attach(Shader::Stage(Shader::Stage::Type::Vertex,
        GetSHExtension() +
        GetForwardVertexExtension()
    ));
    GetShader(Renderer::Options::Pass::ForwardTransparent)->Attach(Shader::Stage(Shader::Stage::Type::Fragment,
        GetForwardFragmentExtension() +
        GetForwardLitPassExtension()
    ));

    SetShader(Renderer::Options::Pass::ShadowDepth, Component::Create<Shader::Program>(GetName() + "_materialShader"));
    GetShader(Renderer::Options::Pass::ShadowDepth)->SetDefine("Pass", "ShadowDepth");
    GetShader(Renderer::Options::Pass::ShadowDepth)->Attach(Shader::Stage(Shader::Stage::Type::Vertex,
        GetForwardVertexExtension()
    ));
    GetShader(Renderer::Options::Pass::ShadowDepth)->Attach(Shader::Stage(Shader::Stage::Type::Fragment,
        GetForwardFragmentExtension() +
        GetCheckOpacityExtension()
    ));

    SetTextureBRDFLUT(Renderer::DefaultBRDFLUT());
    SetOpacity(1);
}

void Material::AddExtension(std::shared_ptr<MaterialExtension> extension)
{
    AddComponent(extension);
    GetShader(Renderer::Options::Pass::DeferredGeometry)->GetStage(Shader::Stage::Type::Fragment) -= GetCheckOpacityExtension();
    GetShader(Renderer::Options::Pass::DeferredGeometry)->GetStage(Shader::Stage::Type::Fragment) += extension->GetCode();
    GetShader(Renderer::Options::Pass::DeferredGeometry)->GetStage(Shader::Stage::Type::Fragment) += GetCheckOpacityExtension();

    GetShader(Renderer::Options::Pass::ForwardTransparent)->GetStage(Shader::Stage::Type::Fragment) -= GetForwardLitPassExtension();
    GetShader(Renderer::Options::Pass::ForwardTransparent)->GetStage(Shader::Stage::Type::Fragment) += extension->GetCode();
    GetShader(Renderer::Options::Pass::ForwardTransparent)->GetStage(Shader::Stage::Type::Fragment) += GetForwardLitPassExtension();

    GetShader(Renderer::Options::Pass::ShadowDepth)->GetStage(Shader::Stage::Type::Fragment) -= GetCheckOpacityExtension();
    GetShader(Renderer::Options::Pass::ShadowDepth)->GetStage(Shader::Stage::Type::Fragment) += extension->GetCode();
    GetShader(Renderer::Options::Pass::ShadowDepth)->GetStage(Shader::Stage::Type::Fragment) += GetCheckOpacityExtension();

}

void Material::RemoveExtension(std::shared_ptr<MaterialExtension> extension)
{
    GetShader(Renderer::Options::Pass::DeferredGeometry)->GetStage(Shader::Stage::Type::Fragment) -= extension->GetCode();
    GetShader(Renderer::Options::Pass::ForwardTransparent)->GetStage(Shader::Stage::Type::Fragment) -= extension->GetCode();
    GetShader(Renderer::Options::Pass::ShadowDepth)->GetStage(Shader::Stage::Type::Fragment) -= extension->GetCode();
    RemoveComponent(extension);
}

std::shared_ptr<MaterialExtension> Material::GetExtension(const std::string& name) const
{
    return GetComponentByName<MaterialExtension>(name);
}

void Material::Bind(const Renderer::Options::Pass& pass)
{
    switch (GetOpacityMode()) {
    case (OpacityMode::Opaque):
        GetShader(pass)->SetDefine("OpacityMode", "Opaque");
        break;
    case (OpacityMode::Mask):
        GetShader(pass)->SetDefine("OpacityMode", "Mask");
        break;
    case (OpacityMode::Blend):
        GetShader(pass)->SetDefine("OpacityMode", "Blend");
        break;
    }
    const auto& extensions { GetComponents<MaterialExtension>() };
    for (const auto& define : GetDefines())
        GetShader(pass)->SetDefine(define.first, define.second);
    for (const auto& extension : extensions) {
        for (const auto& define : extension->GetDefines()) {
            GetShader(pass)->SetDefine(define.first, define.second);
        }
    }
    GetShader(pass)->Use();
    GetShader(pass)->SetUniform("UVScale", GetUVScale());
    for (const auto& color : GetColors()) {
        GetShader(pass)->SetUniform(color.first, color.second);
    }
    for (const auto& value : GetValues()) {
        GetShader(pass)->SetUniform(value.first, value.second);
    }
    for (const auto& texture : GetTextures()) {
        GetShader(pass)->SetTexture(texture.first, texture.second);
    }
    for (const auto& extension : extensions) {
        for (const auto& color : extension->GetColors()) {
            GetShader(pass)->SetUniform(color.first, color.second);
        }
        for (const auto& value : extension->GetValues()) {
            GetShader(pass)->SetUniform(value.first, value.second);
        }
        for (const auto& texture : extension->GetTextures()) {
            GetShader(pass)->SetTexture(texture.first, texture.second);
        }
    }
    GetShader(pass)->Done();
}

std::shared_ptr<Shader::Program> Material::GetShader(const Renderer::Options::Pass& pass)
{
    return _shaders.at(int(pass));
}

void Material::SetShader(const Renderer::Options::Pass& pass, std::shared_ptr<Shader::Program> shader)
{
    _shaders.at(int(pass)) = shader;
}

glm::vec3 Material::GetDiffuse(void)
{
    return GetColor("StandardDiffuse");
}

glm::vec3 Material::GetEmissive(void)
{
    return GetColor("StandardEmissive");
}

float Material::GetOpacityCutoff(void)
{
    return GetValue("StandardOpacityCutoff");
}

float Material::GetOpacity(void)
{
    return GetValue("StandardOpacity");
}

float Material::GetParallax(void)
{
    return GetValue("StandardParallax");
}

float Material::GetIor(void)
{
    return GetValue("StandardIor");
}

std::shared_ptr<Texture2D> Material::GetTextureDiffuse(void)
{
    return GetTexture("StandardTextureDiffuse");
}

std::shared_ptr<Texture2D> Material::GetTextureEmissive(void)
{
    return GetTexture("StandardTextureEmissive");
}

std::shared_ptr<Texture2D> Material::GetTextureNormal(void)
{
    return GetTexture("StandardTextureNormal");
}

std::shared_ptr<Texture2D> Material::GetTextureHeight(void)
{
    return GetTexture("StandardTextureHeight");
}

std::shared_ptr<Texture2D> Material::GetTextureAO(void)
{
    return GetTexture("StandardTextureAO");
}

std::shared_ptr<Texture2D> Material::GetTextureBRDFLUT(void)
{
    return GetTexture("StandardTextureBRDFLUT");
}

void Material::SetOpacityCutoff(float value)
{
    SetValue("StandardOpacityCutoff", value);
}

void Material::SetDiffuse(glm::vec3 value)
{
    SetColor("StandardDiffuse", value);
}

void Material::SetEmissive(glm::vec3 value)
{
    SetColor("StandardEmissive", value);
}

void Material::SetOpacity(float value)
{
    SetValue("StandardOpacity", value);
}

void Material::SetParallax(float value)
{
    SetValue("StandardParallax", value);
}

void Material::SetIor(float value)
{
    SetValue("StandardIor", value);
}

void Material::SetTextureDiffuse(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureDiffuse", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_DIFFUSE") : shader->RemoveDefine("TEXTURE_USE_DIFFUSE");
    }
}

void Material::SetTextureEmissive(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureEmissive", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_EMISSIVE") : shader->RemoveDefine("TEXTURE_USE_EMISSIVE");
    }
}

void Material::SetTextureNormal(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureNormal", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_NORMAL") : shader->RemoveDefine("TEXTURE_USE_NORMAL");
    }
}

void Material::SetTextureHeight(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureHeight", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_HEIGHT") : shader->RemoveDefine("TEXTURE_USE_HEIGHT");
    }
}

void Material::SetTextureAO(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureAO", value);
    for (auto& shader : _shaders) {
        if (shader == nullptr)
            continue;
        value ? shader->SetDefine("TEXTURE_USE_AO") : shader->RemoveDefine("TEXTURE_USE_AO");
    }
}

void Material::SetTextureBRDFLUT(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureBRDFLUT", value);
}
