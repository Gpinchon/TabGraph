/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 15:48:40
*/

#include "Material/Material.hpp"
#include "Material/MaterialExtension.hpp"
#include "Scene/Scene.hpp" // for Environment
#include "Environment.hpp" // for Environment
#include "Parser/GLSL.hpp" // for GLSL, ForwardShader
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Cubemap.hpp"
#include "brdfLUT.hpp"
#include "GL/glew.h" //for GL_TEXTURE1, GL_TEXTURE10, GL_TEXTURE2

/*static std::string depth_vert_code =
#include "depth.vert"
    ;*/

auto GetCheckOpacityExtension() {
    static auto checkOpacity =
#include "checkOpacity.glsl"
        ;
    static auto shaderCode = Component::Create<ShaderCode>(checkOpacity, "CheckOpacity();");
    return shaderCode;
}

auto GetMaterialPassExtension() {
    static auto materialPass =
#include "material.frag"
        ;
    static auto shaderCode = Component::Create<ShaderCode>(materialPass, "ComputeColor();");
    return shaderCode;
}

auto DefaultBRDFLUT() {
    auto static brdf = Component::Create<Texture2D>("brdf", glm::vec2(256, 256), GL_RG, GL_RG8, GL_UNSIGNED_BYTE, brdfLUT);
    brdf->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    brdf->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return brdf;
}

Material::Material(const std::string& name)
    : Component(name), _brdfLUT(AddComponent(DefaultBRDFLUT()))
{
    OpacityModeChanged.ConnectMember(this, &Material::_updateOpacityMode);
    OpacityCutoffChanged.ConnectMember(this, &Material::_updateOpacityCutoff);
    DiffuseChanged.ConnectMember(this, &Material::_updateDiffuse);
    EmissiveChanged.ConnectMember(this, &Material::_updateEmissive);
    UVScaleChanged.ConnectMember(this, &Material::_updateUVScale);
    OpacityChanged.ConnectMember(this, &Material::_updateOpacity);
    ParallaxChanged.ConnectMember(this, &Material::_updateParallax);
    IorChanged.ConnectMember(this, &Material::_updateIor);
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
    _geometryShader = AddComponent(Component::Create<Shader>(Name() + "_geometryShader"));
    GeometryShader()->SetDefine("GEOMETRY");
    GeometryShader()->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(forward_vert_code, "FillVertexData();")));
    GeometryShader()->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(forward_frag_code, "FillFragmentData();")));
    GeometryShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(setInstanceID, "SetInstanceID();"));
    GeometryShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(GetCheckOpacityExtension());
    _materialShader = AddComponent(Component::Create<Shader>(Name() + "_materialShader"));
    MaterialShader()->SetDefine("MATERIAL");
    MaterialShader()->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(forward_vert_code, "FillVertexData();")));
    MaterialShader()->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(forward_frag_code, "FillFragmentData();")));
    MaterialShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(setInstanceID, "SetInstanceID();"));
    MaterialShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(GetMaterialPassExtension());

    _updateOpacityMode(GetOpacityMode());
    _updateOpacityCutoff(GetOpacityCutoff());
    _updateDiffuse(GetDiffuse());
    _updateEmissive(GetEmissive());
    _updateUVScale(GetUVScale());
    _updateOpacity(GetOpacity());
    _updateParallax(GetParallax());
    _updateIor(GetIor());

    MaterialShader()->SetTexture("StandardTextures.BRDFLUT", BRDFLUT());
}

void Material::AddExtension(std::shared_ptr<MaterialExtension> extension)
{
    AddComponent(extension);
    GeometryShader()->Stage(GL_FRAGMENT_SHADER)->RemoveExtension(GetCheckOpacityExtension());
    MaterialShader()->Stage(GL_FRAGMENT_SHADER)->RemoveExtension(GetMaterialPassExtension());
    GeometryShader()->AddExtension(extension->GetShaderExtension());
    MaterialShader()->AddExtension(extension->GetShaderExtension());
    GeometryShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(GetCheckOpacityExtension());
    MaterialShader()->Stage(GL_FRAGMENT_SHADER)->AddExtension(GetMaterialPassExtension());
}

void Material::RemoveExtension(std::shared_ptr<MaterialExtension> extension)
{
    GeometryShader()->RemoveExtension(extension->GetShaderExtension());
    MaterialShader()->RemoveExtension(extension->GetShaderExtension());
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
        MaterialShader()->SetDefine("TEXTURE_USE_METALLICROUGHNESS");
        MaterialShader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        MaterialShader()->RemoveDefine("TEXTURE_USE_METALLIC");
    } else {
        MaterialShader()->RemoveDefine("TEXTURE_USE_METALLICROUGHNESS");
        TextureRoughness() ? MaterialShader()->SetDefine("TEXTURE_USE_ROUGHNESS") : MaterialShader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        TextureMetallic() ? MaterialShader()->SetDefine("TEXTURE_USE_METALLIC") : MaterialShader()->RemoveDefine("TEXTURE_USE_METALLIC");
    }*/

    //shader()->use();
    bind_textures();
    bind_values();
    //shader()->use(false);
}

void Material::bind_textures()
{
    if (Scene::Current()->GetEnvironment() != nullptr) {
        MaterialShader()->SetTexture("Environment.Diffuse", Scene::Current()->GetEnvironment()->GetDiffuse());
        MaterialShader()->SetTexture("Environment.Irradiance", Scene::Current()->GetEnvironment()->GetIrradiance());
    }
}

void Material::bind_values()
{
    for (const auto extension : GetComponents<MaterialExtension>()) {
        for (const auto& uniform : extension->GetShaderExtension()->Uniforms()) {
            GeometryShader()->SetUniform(uniform.second);
            MaterialShader()->SetUniform(uniform.second);
        }
        for (const auto &texture : extension->GetShaderExtension()->Textures()) {
            GeometryShader()->SetTexture(texture.second);
            MaterialShader()->SetTexture(texture.second);
        }
        for (const auto &attribute : extension->GetShaderExtension()->Attributes()) {
            GeometryShader()->SetAttribute(attribute.second);
            MaterialShader()->SetAttribute(attribute.second);
        }
        for (const auto &define : extension->GetShaderExtension()->Defines()) {
            GeometryShader()->SetDefine(define.first, define.second);
            MaterialShader()->SetDefine(define.first, define.second);
        }
    }
}

std::shared_ptr<Shader> Material::GeometryShader()
{
    return GetComponent<Shader>(_geometryShader);
}

std::shared_ptr<Shader> Material::MaterialShader()
{
    return GetComponent<Shader>(_materialShader);
}

std::shared_ptr<Texture2D> Material::TextureDiffuse() const
{
    return GetComponent<Texture2D>(_texture_diffuse);
}

std::shared_ptr<Texture2D> Material::TextureEmissive() const
{
    return GetComponent<Texture2D>(_texture_emissive);
}

std::shared_ptr<Texture2D> Material::TextureNormal() const
{
    return GetComponent<Texture2D>(_texture_normal);
}

std::shared_ptr<Texture2D> Material::TextureHeight() const
{
    return GetComponent<Texture2D>(_texture_height);
}

std::shared_ptr<Texture2D> Material::TextureAO() const
{
    return GetComponent<Texture2D>(_texture_ao);
}

void Material::SetTextureDiffuse(std::shared_ptr<Texture2D> t)
{
    _texture_diffuse = AddComponent(t);
    GeometryShader()->SetTexture("StandardTextures.Diffuse", TextureDiffuse());
    MaterialShader()->SetTexture("StandardTextures.Diffuse", TextureDiffuse());
    TextureDiffuse() ? GeometryShader()->SetDefine("TEXTURE_USE_DIFFUSE") : GeometryShader()->RemoveDefine("TEXTURE_USE_DIFFUSE");
    TextureDiffuse() ? MaterialShader()->SetDefine("TEXTURE_USE_DIFFUSE") : MaterialShader()->RemoveDefine("TEXTURE_USE_DIFFUSE");
}

void Material::SetTextureEmissive(std::shared_ptr<Texture2D> t)
{
    _texture_emissive = AddComponent(t);
    GeometryShader()->SetTexture("StandardTextures.Emissive", TextureEmissive());
    MaterialShader()->SetTexture("StandardTextures.Emissive", TextureEmissive());
    TextureEmissive() ? GeometryShader()->SetDefine("TEXTURE_USE_EMISSIVE") : GeometryShader()->RemoveDefine("TEXTURE_USE_EMISSIVE");
    TextureEmissive() ? MaterialShader()->SetDefine("TEXTURE_USE_EMISSIVE") : MaterialShader()->RemoveDefine("TEXTURE_USE_EMISSIVE");
}

void Material::SetTextureNormal(std::shared_ptr<Texture2D> t)
{
    _texture_normal = AddComponent(t);
    GeometryShader()->SetTexture("StandardTextures.Normal", TextureNormal());
    MaterialShader()->SetTexture("StandardTextures.Normal", TextureNormal());
    TextureNormal() ? GeometryShader()->SetDefine("TEXTURE_USE_NORMAL") : GeometryShader()->RemoveDefine("TEXTURE_USE_NORMAL");
    TextureNormal() ? MaterialShader()->SetDefine("TEXTURE_USE_NORMAL") : MaterialShader()->RemoveDefine("TEXTURE_USE_NORMAL");
}

void Material::SetTextureHeight(std::shared_ptr<Texture2D> t)
{
    _texture_height = AddComponent(t);
    GeometryShader()->SetTexture("StandardTextures.Height", TextureHeight());
    MaterialShader()->SetTexture("StandardTextures.Height", TextureHeight());
    TextureHeight() ? GeometryShader()->SetDefine("TEXTURE_USE_HEIGHT") : GeometryShader()->RemoveDefine("TEXTURE_USE_HEIGHT");
    TextureHeight() ? MaterialShader()->SetDefine("TEXTURE_USE_HEIGHT") : MaterialShader()->RemoveDefine("TEXTURE_USE_HEIGHT");
}

void Material::SetTextureAO(std::shared_ptr<Texture2D> t)
{
    _texture_ao = AddComponent(t);
    GeometryShader()->SetTexture("StandardTextures.AO", TextureAO());
    MaterialShader()->SetTexture("StandardTextures.AO", TextureAO());
    TextureHeight() ? GeometryShader()->SetDefine("TEXTURE_USE_AO") : GeometryShader()->RemoveDefine("TEXTURE_USE_AO");
    TextureHeight() ? MaterialShader()->SetDefine("TEXTURE_USE_AO") : MaterialShader()->RemoveDefine("TEXTURE_USE_AO");
}

void Material::_updateOpacityMode(OpacityModeValue mode)
{
    switch (mode) {
    case (OpacityModeValue::Opaque):
        GeometryShader()->SetDefine("OPACITYMODE", "OPAQUE");
        MaterialShader()->SetDefine("OPACITYMODE", "OPAQUE");
        break;
    case (OpacityModeValue::Mask):
        GeometryShader()->SetDefine("OPACITYMODE", "MASK");
        MaterialShader()->SetDefine("OPACITYMODE", "MASK");
        break;
    case (OpacityModeValue::Blend):
        GeometryShader()->SetDefine("OPACITYMODE", "BLEND");
        MaterialShader()->SetDefine("OPACITYMODE", "BLEND");
        break;
    }
}

void Material::_updateOpacityCutoff(float opacityCutoff)
{
    GeometryShader()->SetUniform("StandardValues.OpacityCutoff", opacityCutoff);
    MaterialShader()->SetUniform("StandardValues.OpacityCutoff", opacityCutoff);
}

void Material::_updateDiffuse(glm::vec3 diffuse)
{
    GeometryShader()->SetUniform("StandardValues.Diffuse", diffuse);
    MaterialShader()->SetUniform("StandardValues.Diffuse", diffuse);
}

void Material::_updateEmissive(glm::vec3 emissive)
{
    GeometryShader()->SetUniform("StandardValues.Emissive", emissive);
    MaterialShader()->SetUniform("StandardValues.Emissive", emissive);
}

void Material::_updateUVScale(glm::vec2 uvScale)
{
    GeometryShader()->SetUniform("UVScale", uvScale);
    MaterialShader()->SetUniform("UVScale", uvScale);
}

void Material::_updateOpacity(float opacity)
{
    GeometryShader()->SetUniform("StandardValues.Opacity", opacity);
    MaterialShader()->SetUniform("StandardValues.Opacity", opacity);
}

void Material::_updateParallax(float parallax)
{
    GeometryShader()->SetUniform("StandardValues.Parallax", parallax);
    MaterialShader()->SetUniform("StandardValues.Parallax", parallax);
}
void Material::_updateIor(float ior)
{
    GeometryShader()->SetUniform("StandardValues.Ior", ior);
    MaterialShader()->SetUniform("StandardValues.Ior", ior);
}

std::shared_ptr<Texture2D> Material::BRDFLUT() const
{
    return GetComponent<Texture2D>(_brdfLUT);
}

void Material::SetBRDFLUT(const std::shared_ptr<Texture2D> &t)
{
    _brdfLUT = AddComponent(t);
    MaterialShader()->SetTexture("StandardTextures.BRDFLUT", BRDFLUT());
}
