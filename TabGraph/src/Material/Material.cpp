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

    GeometryShader()->SetUniform("UVScale", UVScale());
    GeometryShader()->SetUniform("StandardValues.OpacityMode", int(OpacityMode()));
    GeometryShader()->SetUniform("StandardValues.OpacityCutoff", OpacityCutoff());
    GeometryShader()->SetUniform("StandardValues.Diffuse", Diffuse());
    GeometryShader()->SetUniform("StandardValues.Emissive", Emissive());
    GeometryShader()->SetUniform("StandardValues.Opacity", Opacity());
    GeometryShader()->SetUniform("StandardValues.Parallax", Parallax());
    GeometryShader()->SetUniform("StandardValues.Ior", Ior());

    MaterialShader()->SetUniform("UVScale", UVScale());
    MaterialShader()->SetUniform("StandardValues.OpacityMode", int(OpacityMode()));
    MaterialShader()->SetUniform("StandardValues.OpacityCutoff", OpacityCutoff());
    MaterialShader()->SetUniform("StandardValues.Diffuse", Diffuse());
    MaterialShader()->SetUniform("StandardValues.Emissive", Emissive());
    MaterialShader()->SetUniform("StandardValues.Opacity", Opacity());
    MaterialShader()->SetUniform("StandardValues.Parallax", Parallax());
    MaterialShader()->SetUniform("StandardValues.Ior", Ior());
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
        MaterialShader()->SetTexture("Environment.Diffuse", Scene::Current()->GetEnvironment()->diffuse());
        MaterialShader()->SetTexture("Environment.Irradiance", Scene::Current()->GetEnvironment()->irradiance());
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

Material::OpacityModeValue Material::OpacityMode() const
{
    return _opacityMode;
}

void Material::SetOpacityMode(OpacityModeValue mod)
{
    _opacityMode = mod;
    GeometryShader()->SetUniform("StandardValues.OpacityMode", int(_opacityMode));
    MaterialShader()->SetUniform("StandardValues.OpacityMode", int(_opacityMode));
}

float Material::OpacityCutoff() const
{
	return _opacityCutoff;
}

void Material::SetOpacityCutoff(float opacityCutoff)
{
    _opacityCutoff = opacityCutoff;
    GeometryShader()->SetUniform("StandardValues.OpacityCutoff", _opacityCutoff);
    MaterialShader()->SetUniform("StandardValues.OpacityCutoff", _opacityCutoff);
}

glm::vec3 Material::Diffuse() const
{
    return _diffuse;
}

void Material::SetDiffuse(glm::vec3 value)
{
    _diffuse = value;
    GeometryShader()->SetUniform("StandardValues.Diffuse", value);
    MaterialShader()->SetUniform("StandardValues.Diffuse", value);
}

glm::vec3 Material::Emissive() const
{
    return _emissive;
}

void Material::SetEmissive(glm::vec3 value)
{
    _emissive = value;
    GeometryShader()->SetUniform("StandardValues.Emissive", value);
    MaterialShader()->SetUniform("StandardValues.Emissive", value);
}

glm::vec2 Material::UVScale() const
{
    return _uv_scale;
}

void Material::SetUVScale(glm::vec2 value)
{
    _uv_scale = value;
    GeometryShader()->SetUniform("UVScale", value);
    MaterialShader()->SetUniform("UVScale", value);
}

float Material::Opacity() const
{
    return _opacity;
}

void Material::SetOpacity(float value)
{
    _opacity = value;
    GeometryShader()->SetUniform("StandardValues.Opacity", value);
    MaterialShader()->SetUniform("StandardValues.Opacity", value);
}

float Material::Parallax() const
{
    return _parallax;
}

void Material::SetParallax(float value)
{
    _parallax = value;
    GeometryShader()->SetUniform("StandardValues.Parallax", value);
    MaterialShader()->SetUniform("StandardValues.Parallax", value);
}

float Material::Ior() const
{
    return _ior;
}

void Material::SetIor(float value)
{
    _ior = value;
    GeometryShader()->SetUniform("StandardValues.Ior", value);
    MaterialShader()->SetUniform("StandardValues.Ior", value);
}

bool Material::DoubleSided() const
{
    return _doubleSided;
}

void Material::SetDoubleSided(bool doubleSided)
{
    _doubleSided = doubleSided;
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

void Material::_FixedUpdateGPU(float delta)
{
    GeometryShader()->FixedUpdateGPU(delta);
    MaterialShader()->FixedUpdateGPU(delta);
}
