/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 22:47:48
*/

#include "Material/Material.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Environment.hpp" // for Environment
#include "Material/MaterialExtension.hpp"
#include "Scene/Scene.hpp" // for Environment
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Cubemap.hpp"
#include "Texture/Texture2D.hpp"
#include "brdfLUT.hpp"

#include <GL/glew.h> //for GL_TEXTURE1, GL_TEXTURE10, GL_TEXTURE2

/*static std::string depth_vert_code =
#include "depth.vert"
    ;*/

auto GetCheckOpacityExtension()
{
    static auto checkOpacity =
#include "checkOpacity.glsl"
        ;
    static auto shaderCode = Component::Create<ShaderCode>(checkOpacity, "CheckOpacity();");
    return shaderCode;
}

auto GetMaterialPassExtension()
{
    static auto materialPass =
#include "material.frag"
        ;
    static auto shaderCode = Component::Create<ShaderCode>(materialPass, "ComputeColor();");
    return shaderCode;
}

auto DefaultBRDFLUT()
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
    static std::string forward_vert_code =
#include "forward.vert"
        ;
    static std::string forward_frag_code =
#include "forward.frag"
        ;
    static std::string material_frag_code =
#include "material.frag"
        ;

    static auto setInstanceID =
#include "setInstanceID.glsl"
        ;
    SetGeometryShader(Component::Create<Shader>(GetName() + "_geometryShader"));
    GetGeometryShader()->SetDefine("GEOMETRY");
    GetGeometryShader()->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(forward_vert_code, "FillVertexData();")));
    GetGeometryShader()->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(forward_frag_code, "FillFragmentData();")));
    GetGeometryShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(setInstanceID, "SetInstanceID();"));
    GetGeometryShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(GetCheckOpacityExtension());
    SetMaterialShader(Component::Create<Shader>(GetName() + "_materialShader"));
    GetMaterialShader()->SetDefine("MATERIAL");
    GetMaterialShader()->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(forward_vert_code, "FillVertexData();")));
    GetMaterialShader()->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(forward_frag_code, "FillFragmentData();")));
    GetMaterialShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(setInstanceID, "SetInstanceID();"));
    GetMaterialShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(GetMaterialPassExtension());

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
    GetGeometryShader()->Stage(GL_FRAGMENT_SHADER)->RemoveExtension(GetCheckOpacityExtension());
    GetMaterialShader()->Stage(GL_FRAGMENT_SHADER)->RemoveExtension(GetMaterialPassExtension());
    GetGeometryShader()->AddExtension(extension->GetShaderExtension());
    GetMaterialShader()->AddExtension(extension->GetShaderExtension());
    GetGeometryShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(GetCheckOpacityExtension());
    GetMaterialShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(GetMaterialPassExtension());
}

void Material::RemoveExtension(std::shared_ptr<MaterialExtension> extension)
{
    GetGeometryShader()->RemoveExtension(extension->GetShaderExtension());
    GetMaterialShader()->RemoveExtension(extension->GetShaderExtension());
    RemoveComponent(extension);
}

std::shared_ptr<MaterialExtension> Material::GetExtension(const std::string& name) const
{
    return GetComponentByName<MaterialExtension>(name);
}

void Material::Bind()
{
    /*if (nullptr == shader())
        return;*/
    /*if (TextureMetallicRoughness() != nullptr) {
        GetMaterialShader()->SetDefine("TEXTURE_USE_METALLICROUGHNESS");
        GetMaterialShader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        GetMaterialShader()->RemoveDefine("TEXTURE_USE_METALLIC");
    } else {
        GetMaterialShader()->RemoveDefine("TEXTURE_USE_METALLICROUGHNESS");
        TextureRoughness() ? GetMaterialShader()->SetDefine("TEXTURE_USE_ROUGHNESS") : GetMaterialShader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        TextureMetallic() ? GetMaterialShader()->SetDefine("TEXTURE_USE_METALLIC") : GetMaterialShader()->RemoveDefine("TEXTURE_USE_METALLIC");
    }*/

    //shader()->use();
    bind_textures();
    bind_values();
    //shader()->use(false);
}

void Material::SetGeometryShader(std::shared_ptr<Shader> shader)
{
    _SetGeometryShader(shader);
}

void Material::SetMaterialShader(std::shared_ptr<Shader> shader)
{
    _SetMaterialShader(shader);
}

void Material::bind_textures()
{
    if (Scene::Current()->GetEnvironment() != nullptr) {
        GetMaterialShader()->SetTexture("Environment.Diffuse", Scene::Current()->GetEnvironment()->GetDiffuse());
        GetMaterialShader()->SetTexture("Environment.Irradiance", Scene::Current()->GetEnvironment()->GetIrradiance());
    }
}

void Material::bind_values()
{
    for (const auto extension : GetComponents<MaterialExtension>()) {
        for (const auto& uniform : extension->GetShaderExtension()->Uniforms()) {
            GetGeometryShader()->SetUniform(uniform.second);
            GetMaterialShader()->SetUniform(uniform.second);
        }
        for (const auto& texture : extension->GetShaderExtension()->Textures()) {
            GetGeometryShader()->SetTexture(texture.second);
            GetMaterialShader()->SetTexture(texture.second);
        }
        for (const auto& attribute : extension->GetShaderExtension()->Attributes()) {
            GetGeometryShader()->SetAttribute(attribute.second);
            GetMaterialShader()->SetAttribute(attribute.second);
        }
        for (const auto& define : extension->GetShaderExtension()->Defines()) {
            GetGeometryShader()->SetDefine(define.first, define.second);
            GetMaterialShader()->SetDefine(define.first, define.second);
        }
    }
}

void Material::SetTextureDiffuse(std::shared_ptr<Texture2D> t)
{
    GetGeometryShader()->SetTexture("StandardTextures.Diffuse", t);
    GetMaterialShader()->SetTexture("StandardTextures.Diffuse", t);
    t ? GetGeometryShader()->SetDefine("TEXTURE_USE_DIFFUSE") : GetGeometryShader()->RemoveDefine("TEXTURE_USE_DIFFUSE");
    t ? GetMaterialShader()->SetDefine("TEXTURE_USE_DIFFUSE") : GetMaterialShader()->RemoveDefine("TEXTURE_USE_DIFFUSE");
    _SetTextureDiffuse(t);
}

void Material::SetTextureEmissive(std::shared_ptr<Texture2D> t)
{
    GetGeometryShader()->SetTexture("StandardTextures.Emissive", t);
    GetMaterialShader()->SetTexture("StandardTextures.Emissive", t);
    t ? GetGeometryShader()->SetDefine("TEXTURE_USE_EMISSIVE") : GetGeometryShader()->RemoveDefine("TEXTURE_USE_EMISSIVE");
    t ? GetMaterialShader()->SetDefine("TEXTURE_USE_EMISSIVE") : GetMaterialShader()->RemoveDefine("TEXTURE_USE_EMISSIVE");
    _SetTextureEmissive(t);
}

void Material::SetTextureNormal(std::shared_ptr<Texture2D> t)
{
    GetGeometryShader()->SetTexture("StandardTextures.Normal", t);
    GetMaterialShader()->SetTexture("StandardTextures.Normal", t);
    t ? GetGeometryShader()->SetDefine("TEXTURE_USE_NORMAL") : GetGeometryShader()->RemoveDefine("TEXTURE_USE_NORMAL");
    t ? GetMaterialShader()->SetDefine("TEXTURE_USE_NORMAL") : GetMaterialShader()->RemoveDefine("TEXTURE_USE_NORMAL");
    _SetTextureNormal(t);
}

void Material::SetTextureHeight(std::shared_ptr<Texture2D> t)
{
    GetGeometryShader()->SetTexture("StandardTextures.Height", t);
    GetMaterialShader()->SetTexture("StandardTextures.Height", t);
    t ? GetGeometryShader()->SetDefine("TEXTURE_USE_HEIGHT") : GetGeometryShader()->RemoveDefine("TEXTURE_USE_HEIGHT");
    t ? GetMaterialShader()->SetDefine("TEXTURE_USE_HEIGHT") : GetMaterialShader()->RemoveDefine("TEXTURE_USE_HEIGHT");
    _SetTextureHeight(t);
}

void Material::SetTextureAO(std::shared_ptr<Texture2D> t)
{
    GetGeometryShader()->SetTexture("StandardTextures.AO", t);
    GetMaterialShader()->SetTexture("StandardTextures.AO", t);
    t ? GetGeometryShader()->SetDefine("TEXTURE_USE_AO") : GetGeometryShader()->RemoveDefine("TEXTURE_USE_AO");
    t ? GetMaterialShader()->SetDefine("TEXTURE_USE_AO") : GetMaterialShader()->RemoveDefine("TEXTURE_USE_AO");
    _SetTextureAO(t);
}

void Material::SetOpacityMode(OpacityModeValue mode)
{
    switch (mode) {
    case (OpacityModeValue::Opaque):
        GetGeometryShader()->SetDefine("OPACITYMODE", "OPAQUE");
        GetMaterialShader()->SetDefine("OPACITYMODE", "OPAQUE");
        break;
    case (OpacityModeValue::Mask):
        GetGeometryShader()->SetDefine("OPACITYMODE", "MASK");
        GetMaterialShader()->SetDefine("OPACITYMODE", "MASK");
        break;
    case (OpacityModeValue::Blend):
        GetGeometryShader()->SetDefine("OPACITYMODE", "BLEND");
        GetMaterialShader()->SetDefine("OPACITYMODE", "BLEND");
        break;
    }
    _SetOpacityMode(mode);
}

void Material::SetOpacityCutoff(float opacityCutoff)
{
    GetGeometryShader()->SetUniform("StandardValues.OpacityCutoff", opacityCutoff);
    GetMaterialShader()->SetUniform("StandardValues.OpacityCutoff", opacityCutoff);
    _SetOpacityCutoff(opacityCutoff);
}

void Material::SetDiffuse(glm::vec3 diffuse)
{
    GetGeometryShader()->SetUniform("StandardValues.Diffuse", diffuse);
    GetMaterialShader()->SetUniform("StandardValues.Diffuse", diffuse);
    _SetDiffuse(diffuse);
}

void Material::SetEmissive(glm::vec3 emissive)
{
    GetGeometryShader()->SetUniform("StandardValues.Emissive", emissive);
    GetMaterialShader()->SetUniform("StandardValues.Emissive", emissive);
    _SetEmissive(emissive);
}

void Material::SetUVScale(glm::vec2 uvScale)
{
    GetGeometryShader()->SetUniform("UVScale", uvScale);
    GetMaterialShader()->SetUniform("UVScale", uvScale);
    _SetUVScale(uvScale);
}

void Material::SetOpacity(float opacity)
{
    GetGeometryShader()->SetUniform("StandardValues.Opacity", opacity);
    GetMaterialShader()->SetUniform("StandardValues.Opacity", opacity);
    _SetOpacity(opacity);
}

void Material::SetParallax(float parallax)
{
    GetGeometryShader()->SetUniform("StandardValues.Parallax", parallax);
    GetMaterialShader()->SetUniform("StandardValues.Parallax", parallax);
    _SetParallax(parallax);
}
void Material::SetIor(float ior)
{
    GetGeometryShader()->SetUniform("StandardValues.Ior", ior);
    GetMaterialShader()->SetUniform("StandardValues.Ior", ior);
    _SetIor(ior);
}

void Material::SetTextureBRDFLUT(const std::shared_ptr<Texture2D>& t)
{
    GetMaterialShader()->SetTexture("StandardTextures.BRDFLUT", t);
    _SetTextureBRDFLUT(t);
}
