/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-24 17:34:48
*/

#include "Material/Material.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Environment.hpp" // for Environment
#include "Material/MaterialExtension.hpp"
#include "Scene/Scene.hpp" // for Environment
#include "Shader/Program.hpp" // for Shader::Program
#include "Shader/Stage.hpp" // for Shader::Stage
#include "Texture/Cubemap.hpp"
#include "Texture/Texture2D.hpp"
#include "brdfLUT.hpp"

/*static std::string depth_vert_code =
#include "depth.vert"
    ;*/

static inline auto& GetCheckOpacityExtension()
{
    static auto checkOpacity =
#include "checkOpacity.glsl"
        ;
    static Shader::Stage::Code shaderCode { checkOpacity, "CheckOpacity();" }; //Component::Create<ShaderCode>(checkOpacity, "CheckOpacity();");
    return shaderCode;
}

static inline auto& GetMaterialPassExtension()
{
    static auto materialPass =
#include "material.frag"
        ;
    static Shader::Stage::Code shaderCode { materialPass, "ComputeColor();" }; // Component::Create<ShaderCode>(materialPass, "ComputeColor();");
    return shaderCode;
}

static inline auto& GetInstanceIDExtension()
{
    static auto setInstanceID =
#include "setInstanceID.glsl"
        ;
    static Shader::Stage::Code shaderCode { setInstanceID, "SetInstanceID();" }; // Component::Create<ShaderCode>(materialPass, "ComputeColor();");
    return shaderCode;
}

static inline auto& GetForwardVertexExtension()
{
    static auto forward_vert_code =
#include "forward.vert"
        ;
    static Shader::Stage::Code shaderCode { forward_vert_code, "FillVertexData();" }; // Component::Create<ShaderCode>(materialPass, "ComputeColor();");
    return shaderCode;
}

static inline auto& GetForwardFragmentExtension()
{
    static auto forward_frag_code =
#include "forward.frag"
        ;
    static Shader::Stage::Code shaderCode { forward_frag_code, "FillFragmentData();" }; // Component::Create<ShaderCode>(materialPass, "ComputeColor();");
    return shaderCode;
}

static inline auto& DefaultBRDFLUT()
{
    static std::shared_ptr<Texture2D> brdf;
    if (brdf == nullptr) {
        auto brdfImageAsset(Component::Create<Asset>());
        brdf = Component::Create<Texture2D>(brdfImageAsset);
        auto brdfImage { Component::Create<Image>(glm::vec2(256, 256), Pixel::SizedFormat::Uint8_NormalizedRG, brdfLUT) };
        brdfImageAsset->SetComponent(brdfImage);
        brdfImageAsset->SetLoaded(true);
        brdf->SetName("BrdfLUT");
        brdf->SetParameter<Texture::Parameter::WrapS>(Texture::Wrap::ClampToEdge);
        brdf->SetParameter<Texture::Parameter::WrapT>(Texture::Wrap::ClampToEdge);
    }
    return brdf;
}

Material::Material(const std::string& name)
    : MaterialExtension(name)
{
    static std::string material_frag_code =
#include "material.frag"
        ;
    SetShader(Render::Pass::Geometry, Component::Create<Shader::Program>(GetName() + "_geometryShader"));
    GetShader(Render::Pass::Geometry)->SetDefine("GEOMETRY");
    GetShader(Render::Pass::Geometry)->Attach(Shader::Stage(Shader::Stage::Type::Vertex, GetForwardVertexExtension()));
    GetShader(Render::Pass::Geometry)->Attach(Shader::Stage(Shader::Stage::Type::Fragment, GetForwardFragmentExtension() + GetInstanceIDExtension() + GetCheckOpacityExtension()));

    SetShader(Render::Pass::Material, Component::Create<Shader::Program>(GetName() + "_materialShader"));
    GetShader(Render::Pass::Material)->SetDefine("MATERIAL");
    GetShader(Render::Pass::Material)->Attach(Shader::Stage(Shader::Stage::Type::Vertex, GetForwardVertexExtension()));
    GetShader(Render::Pass::Material)->Attach(Shader::Stage(Shader::Stage::Type::Fragment, GetForwardFragmentExtension() + GetInstanceIDExtension() + GetMaterialPassExtension()));
    SetTextureBRDFLUT(DefaultBRDFLUT());
}

void Material::AddExtension(std::shared_ptr<MaterialExtension> extension)
{
    AddComponent(extension);
    GetShader(Render::Pass::Geometry)->GetStage(Shader::Stage::Type::Fragment) -= GetCheckOpacityExtension();
    GetShader(Render::Pass::Material)->GetStage(Shader::Stage::Type::Fragment) -= GetMaterialPassExtension();
    GetShader(Render::Pass::Geometry)->GetStage(Shader::Stage::Type::Fragment) += extension->GetCode();
    GetShader(Render::Pass::Material)->GetStage(Shader::Stage::Type::Fragment) += extension->GetCode();
    GetShader(Render::Pass::Geometry)->GetStage(Shader::Stage::Type::Fragment) += GetCheckOpacityExtension();
    GetShader(Render::Pass::Material)->GetStage(Shader::Stage::Type::Fragment) += GetMaterialPassExtension();
}

void Material::RemoveExtension(std::shared_ptr<MaterialExtension> extension)
{
    GetShader(Render::Pass::Geometry)->GetStage(Shader::Stage::Type::Fragment) -= extension->GetCode();
    GetShader(Render::Pass::Material)->GetStage(Shader::Stage::Type::Fragment) -= extension->GetCode();
    RemoveComponent(extension);
}

std::shared_ptr<MaterialExtension> Material::GetExtension(const std::string& name) const
{
    return GetComponentByName<MaterialExtension>(name);
}

void Material::Bind(const Render::Pass& pass)
{
    switch (GetOpacityMode()) {
    case (OpacityModeValue::Opaque):
        GetShader(pass)->SetDefine("OPACITYMODE", "OPAQUE");
        break;
    case (OpacityModeValue::Mask):
        GetShader(pass)->SetDefine("OPACITYMODE", "MASK");
        break;
    case (OpacityModeValue::Blend):
        GetShader(pass)->SetDefine("OPACITYMODE", "BLEND");
        break;
    }
    const auto& extensions{ GetComponents<MaterialExtension>() };
    for (const auto &define : GetDefines())
        GetShader(pass)->SetDefine(define.first, define.second);
    for (const auto &extension : extensions) {
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
    for (const auto &extension : extensions) {
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
    if (Scene::Current()->GetEnvironment() != nullptr) {
        GetShader(pass)->SetTexture("Environment.Diffuse", Scene::Current()->GetEnvironment()->GetDiffuse());
        GetShader(pass)->SetTexture("Environment.Irradiance", Scene::Current()->GetEnvironment()->GetIrradiance());
    }
    GetShader(pass)->Done();
}

std::shared_ptr<Shader::Program> Material::GetShader(const Render::Pass& pass)
{
    return _shaders.at(int(pass));
}

void Material::SetShader(const Render::Pass& pass, std::shared_ptr<Shader::Program> shader)
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
    for (auto &shader : _shaders)
        value ? shader->SetDefine("TEXTURE_USE_DIFFUSE") : shader->RemoveDefine("TEXTURE_USE_DIFFUSE");
}

void Material::SetTextureEmissive(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureEmissive", value);
    for (auto& shader : _shaders)
        value ? shader->SetDefine("TEXTURE_USE_EMISSIVE") : shader->RemoveDefine("TEXTURE_USE_EMISSIVE");
}

void Material::SetTextureNormal(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureNormal", value);
    for (auto& shader : _shaders)
        value ? shader->SetDefine("TEXTURE_USE_NORMAL") : shader->RemoveDefine("TEXTURE_USE_NORMAL");
}

void Material::SetTextureHeight(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureHeight", value);
    for (auto& shader : _shaders)
        value ? shader->SetDefine("TEXTURE_USE_HEIGHT") : shader->RemoveDefine("TEXTURE_USE_HEIGHT");
}

void Material::SetTextureAO(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureAO", value);
    for (auto& shader : _shaders)
        value ? shader->SetDefine("TEXTURE_USE_AO") : shader->RemoveDefine("TEXTURE_USE_AO");
}

void Material::SetTextureBRDFLUT(std::shared_ptr<Texture2D> value)
{
    SetTexture("StandardTextureBRDFLUT", value);
}
