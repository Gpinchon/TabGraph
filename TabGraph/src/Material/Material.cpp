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
    : Component(name)
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

    SetOpacityMode(GetOpacityMode());
    SetOpacityCutoff(GetOpacityCutoff());
    SetDiffuse(GetDiffuse());
    SetEmissive(GetEmissive());
    SetUVScale(GetUVScale());
    SetOpacity(GetOpacity());
    SetParallax(GetParallax());
    SetIor(GetIor());
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
    //shader()->use();
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
    GetTextureDiffuse() ? GetShader(pass)->SetDefine("TEXTURE_USE_DIFFUSE") : GetShader(pass)->RemoveDefine("TEXTURE_USE_DIFFUSE");
    GetTextureEmissive() ? GetShader(pass)->SetDefine("TEXTURE_USE_EMISSIVE") : GetShader(pass)->RemoveDefine("TEXTURE_USE_EMISSIVE");
    GetTextureNormal() ? GetShader(pass)->SetDefine("TEXTURE_USE_NORMAL") : GetShader(pass)->RemoveDefine("TEXTURE_USE_NORMAL");
    GetTextureHeight() ? GetShader(pass)->SetDefine("TEXTURE_USE_HEIGHT") : GetShader(pass)->RemoveDefine("TEXTURE_USE_HEIGHT");
    GetTextureAO() ? GetShader(pass)->SetDefine("TEXTURE_USE_AO") : GetShader(pass)->RemoveDefine("TEXTURE_USE_AO");
    for (const auto extension : GetComponents<MaterialExtension>()) {
        for (const auto& define : extension->GetDefines()) {
            GetShader(pass)->SetDefine(define.first, define.second);
        }
    }
    GetShader(pass)->Use();
    bind_textures(pass);
    bind_values(pass);
    GetShader(pass)->Done();
    //shader()->use(false);
}

std::shared_ptr<Shader::Program> Material::GetShader(const Render::Pass& pass)
{
    return _shaders.at(int(pass));
}

void Material::SetShader(const Render::Pass& pass, std::shared_ptr<Shader::Program> shader)
{
    _shaders.at(int(pass)) = shader;
}

void Material::bind_textures(const Render::Pass& pass)
{
    if (Scene::Current()->GetEnvironment() != nullptr) {
        GetShader(pass)->SetTexture("Environment.Diffuse", Scene::Current()->GetEnvironment()->GetDiffuse());
        GetShader(pass)->SetTexture("Environment.Irradiance", Scene::Current()->GetEnvironment()->GetIrradiance());
    }
    GetShader(pass)->SetTexture("StandardTextures.Diffuse", GetTextureDiffuse());
    GetShader(pass)->SetTexture("StandardTextures.Emissive", GetTextureEmissive());
    GetShader(pass)->SetTexture("StandardTextures.Normal", GetTextureNormal());
    GetShader(pass)->SetTexture("StandardTextures.Height", GetTextureHeight());
    GetShader(pass)->SetTexture("StandardTextures.AO", GetTextureAO());
    GetShader(pass)->SetTexture("StandardTextures.BRDFLUT", GetTextureBRDFLUT());
    for (const auto extension : GetComponents<MaterialExtension>()) {
        for (const auto& texture : extension->GetTextures()) {
            GetShader(pass)->SetTexture(texture.first, texture.second);
        }
    }
}

void Material::bind_values(const Render::Pass& pass)
{
    GetShader(pass)->SetUniform("StandardOpacityCutoff", GetOpacityCutoff());
    GetShader(pass)->SetUniform("StandardDiffuse",GetDiffuse());
    GetShader(pass)->SetUniform("StandardEmissive", GetEmissive());
    GetShader(pass)->SetUniform("StandardOpacity", GetOpacity());
    GetShader(pass)->SetUniform("StandardParallax", GetParallax());
    GetShader(pass)->SetUniform("StandardIor", GetIor());
    GetShader(pass)->SetUniform("UVScale", GetUVScale());
    for (const auto extension : GetComponents<MaterialExtension>()) {
        for (const auto& color : extension->GetColors()) {
            GetShader(pass)->SetUniform(color.first, color.second);
        }
        for (const auto& value : extension->GetValues()) {
            GetShader(pass)->SetUniform(value.first, value.second);
        }
    }
}

void Material::SetOpacityMode(OpacityModeValue value)
{
    _SetOpacityMode(value);
}

void Material::SetOpacityCutoff(float value)
{
    _SetOpacityCutoff(value);
}

void Material::SetDiffuse(glm::vec3 value)
{
    _SetDiffuse(value);
}

void Material::SetEmissive(glm::vec3 value)
{
    _SetEmissive(value);
}

void Material::SetUVScale(glm::vec2 value)
{
    _SetUVScale(value);
}

void Material::SetOpacity(float value)
{
    _SetOpacity(value);
}

void Material::SetParallax(float value)
{
    _SetParallax(value);
}

void Material::SetIor(float value)
{
    _SetIor(value);
}

void Material::SetTextureDiffuse(std::shared_ptr<Texture2D> value)
{
    _SetTextureDiffuse(value);
}

void Material::SetTextureEmissive(std::shared_ptr<Texture2D> value)
{
    _SetTextureEmissive(value);
}

void Material::SetTextureNormal(std::shared_ptr<Texture2D> value)
{
    _SetTextureNormal(value);
}

void Material::SetTextureHeight(std::shared_ptr<Texture2D> value)
{
    _SetTextureHeight(value);
}

void Material::SetTextureAO(std::shared_ptr<Texture2D> value)
{
    _SetTextureAO(value);
}

void Material::SetTextureBRDFLUT(const std::shared_ptr<Texture2D>& value)
{
    _SetTextureBRDFLUT(value);
}
