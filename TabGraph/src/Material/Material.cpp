/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 15:48:40
*/

#include "Material/Material.hpp"
#include "Material/MaterialExtension.hpp"
#include "Environment.hpp" // for Environment
#include "Parser/GLSL.hpp" // for GLSL, ForwardShader
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Cubemap.hpp"
#include <GL/glew.h> // for GL_TEXTURE1, GL_TEXTURE10, GL_TEXTURE2

/*static std::string depth_vert_code =
#include "depth.vert"
    ;*/

Material::Material(const std::string& name)
    : Component(name)
{
    static auto forward_default_frag_technique =
#include "forward_default.frag"
        ;
    static auto depth_frag_code =
#include "depth.frag"
        ;
    static auto defaultFragmentCode = ShaderCode::Create(forward_default_frag_technique, "CheckOpacity();");
    _shader = Shader::Create(Name() + "_shader", ForwardShader);
    _shader->Stage(GL_FRAGMENT_SHADER)->AddExtension(defaultFragmentCode);
    static auto defaultFragmentDepthCode = ShaderCode::Create(forward_default_frag_technique, "CheckOpacity();");
    _depth_shader = Shader::Create(Name() + "_depth_shader", ForwardShader);
    _depth_shader->Stage(GL_FRAGMENT_SHADER)->AddExtension(defaultFragmentDepthCode);
    //_depth_shader->SetStage(ShaderStage::Create(GL_FRAGMENT_SHADER, depth_frag_code));
    shader()->SetUniform("UVScale", UVScale());
    shader()->SetUniform("StandardValues.Diffuse", Diffuse());
    shader()->SetUniform("StandardValues.Emissive", Emissive());
    shader()->SetUniform("StandardValues.Opacity", Opacity());
    shader()->SetUniform("StandardValues.Parallax", Parallax());
    shader()->SetUniform("StandardValues.Ior", Ior());
    depth_shader()->SetUniform("UVScale", UVScale());
    depth_shader()->SetUniform("StandardValues.Diffuse", Diffuse());
    depth_shader()->SetUniform("StandardValues.Emissive", Emissive());
    depth_shader()->SetUniform("StandardValues.Opacity", Opacity());
    depth_shader()->SetUniform("StandardValues.Parallax", Parallax());
    depth_shader()->SetUniform("StandardValues.Ior", Ior());
}

std::shared_ptr<Material> Material::Create(const std::string& name)
{
    auto mtl = std::shared_ptr<Material>(new Material(name));
    return mtl;
}

void Material::AddExtension(std::shared_ptr<MaterialExtension> extension)
{
    shader()->AddExtension(extension->GetShaderExtension());
    AddComponent(extension);
}

std::shared_ptr<MaterialExtension> Material::GetExtension(const std::string& name) const
{
    return GetComponentByName<MaterialExtension>(name);
}

void Material::Bind()
{
    if (nullptr == shader())
        return;
    /*
    if (TextureMetallicRoughness() != nullptr) {
        shader()->SetDefine("TEXTURE_USE_METALLICROUGHNESS");
        shader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        shader()->RemoveDefine("TEXTURE_USE_METALLIC");
    } else {
        shader()->RemoveDefine("TEXTURE_USE_METALLICROUGHNESS");
        TextureRoughness() ? shader()->SetDefine("TEXTURE_USE_ROUGHNESS") : shader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        TextureMetallic() ? shader()->SetDefine("TEXTURE_USE_METALLIC") : shader()->RemoveDefine("TEXTURE_USE_METALLIC");
    }*/

    //shader()->use();
    bind_textures();
    bind_values();
    //shader()->use(false);
}

void Material::SetShader(std::shared_ptr<Shader> shader)
{
    _shader = shader;
    _shaderChanged = true;
}

void Material::bind_textures()
{
    if (Environment::current() != nullptr) {
        shader()->SetTexture("Environment.Diffuse", Environment::current()->diffuse());
        shader()->SetTexture("Environment.Irradiance", Environment::current()->irradiance());
    }
}

void Material::bind_values()
{
    for (const auto extension : GetComponents<MaterialExtension>()) {
        for (const auto& uniform : extension->GetShaderExtension()->Uniforms()) {
            shader()->SetUniform(uniform.second);
        }
        for (const auto &texture : extension->GetShaderExtension()->Textures()) {
            shader()->SetTexture(texture.second);
        }
        for (const auto &attribute : extension->GetShaderExtension()->Attributes()) {
            shader()->SetAttribute(attribute.second);
        }
        for (const auto &define : extension->GetShaderExtension()->Defines()) {
            shader()->SetDefine(define.first, define.second);
        }
    }
}

std::shared_ptr<Shader> Material::shader()
{
    return _shader;
}

std::shared_ptr<Shader> Material::depth_shader()
{
    return _depth_shader;
}

std::shared_ptr<Texture2D> Material::TextureDiffuse() const
{
    return _texture_diffuse;
}

std::shared_ptr<Texture2D> Material::TextureEmissive() const
{
    return _texture_emissive;
}

std::shared_ptr<Texture2D> Material::TextureNormal() const
{
    return _texture_normal;
}

std::shared_ptr<Texture2D> Material::TextureHeight() const
{
    return _texture_height;
}

std::shared_ptr<Texture2D> Material::TextureAO() const
{
    return _texture_ao;
}

void Material::SetTextureDiffuse(std::shared_ptr<Texture2D> t)
{
    _texture_diffuse = t;
    shader()->SetTexture("StandardTextures.Diffuse", TextureDiffuse());
    TextureDiffuse() ? shader()->SetDefine("TEXTURE_USE_DIFFUSE") : shader()->RemoveDefine("TEXTURE_USE_DIFFUSE");
}

void Material::SetTextureEmissive(std::shared_ptr<Texture2D> t)
{
    _texture_emissive = t;
    shader()->SetTexture("StandardTextures.Emissive", TextureEmissive());
    TextureEmissive() ? shader()->SetDefine("TEXTURE_USE_EMISSIVE") : shader()->RemoveDefine("TEXTURE_USE_EMISSIVE");
}

void Material::SetTextureNormal(std::shared_ptr<Texture2D> t)
{
    _texture_normal = t;
    shader()->SetTexture("StandardTextures.Normal", TextureNormal());
    TextureNormal() ? shader()->SetDefine("TEXTURE_USE_NORMAL") : shader()->RemoveDefine("TEXTURE_USE_NORMAL");
}

void Material::SetTextureHeight(std::shared_ptr<Texture2D> t)
{
    _texture_height = t;
    shader()->SetTexture("StandardTextures.Height", TextureHeight());
    TextureHeight() ? shader()->SetDefine("TEXTURE_USE_HEIGHT") : shader()->RemoveDefine("TEXTURE_USE_HEIGHT");
}

void Material::SetTextureAO(std::shared_ptr<Texture2D> t)
{
    _texture_ao = t;
    shader()->SetTexture("StandardTextures.AO", TextureAO());
    TextureHeight() ? shader()->SetDefine("TEXTURE_USE_AO") : shader()->RemoveDefine("TEXTURE_USE_AO");
}

glm::vec3 Material::Diffuse() const
{
    return _diffuse;
}

void Material::SetDiffuse(glm::vec3 value)
{
    shader()->SetUniform("StandardValues.Diffuse", value);
    _diffuse = value;
}

glm::vec3 Material::Emissive() const
{
    return _emissive;
}

void Material::SetEmissive(glm::vec3 value)
{
    shader()->SetUniform("StandardValues.Emissive", value);
    _emissive = value;
}

glm::vec2 Material::UVScale() const
{
    return _uv_scale;
}

void Material::SetUVScale(glm::vec2 value)
{
    shader()->SetUniform("UVScale", value);
    _uv_scale = value;
}

float Material::Opacity() const
{
    return _opacity;
}

void Material::SetOpacity(float value)
{
    shader()->SetUniform("StandardValues.Opacity", value);
    _opacity = value;
}

float Material::Parallax() const
{
    return _parallax;
}

void Material::SetParallax(float value)
{
    shader()->SetUniform("StandardValues.Parallax", value);
    _parallax = value;
}

float Material::Ior() const
{
    return _ior;
}

void Material::SetIor(float value)
{
    shader()->SetUniform("StandardValues.Ior", value);
    _ior = value;
}

bool Material::DoubleSided() const
{
    return _doubleSided;
}

void Material::SetDoubleSided(bool doubleSided)
{
    _doubleSided = doubleSided;
}

void Material::_FixedUpdateGPU(float delta)
{
    _shader->FixedUpdateGPU(delta);
    _depth_shader->FixedUpdateGPU(delta);
}
