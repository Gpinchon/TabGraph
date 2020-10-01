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

static std::string forward_default_frag_technique =
#include "forward_default.frag"
    ;

static std::string depth_vert_code =
#include "depth.vert"
    ;

static std::string depth_frag_code =
#include "depth.frag"
    ;

Material::Material(const std::string& name)
    : Component(name)
{
    _shader = Shader::Create(Name() + "_shader", ForwardShader);
    _shader.lock()->Stage(GL_FRAGMENT_SHADER)->SetTechnique(forward_default_frag_technique);
    _depth_shader = Shader::Create(Name() + "_depth_shader", ForwardShader);
    _depth_shader.lock()->SetStage(ShaderStage::Create(GL_FRAGMENT_SHADER, depth_frag_code));
    shader()->SetUniform("UVScale", UVScale());
    shader()->SetUniform("_StandardValues.Emissive", Emissive());
    shader()->SetUniform("_StandardValues.Opacity", Opacity());
    shader()->SetUniform("_StandardValues.Parallax", Parallax());
    shader()->SetUniform("_StandardValues.Ior", Ior());
    depth_shader()->SetUniform("UVScale", UVScale());
    depth_shader()->SetUniform("_StandardValues.Emissive", Emissive());
    depth_shader()->SetUniform("_StandardValues.Opacity", Opacity());
    depth_shader()->SetUniform("_StandardValues.Parallax", Parallax());
    depth_shader()->SetUniform("_StandardValues.Ior", Ior());
}

std::shared_ptr<Material> Material::Create(const std::string& name)
{
    auto mtl = std::shared_ptr<Material>(new Material(name));
    return mtl;
}

void Material::AddExtension(std::shared_ptr<MaterialExtension> extension)
{
    AddComponent(extension);
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

void Material::bind_textures()
{
    if (Environment::current() != nullptr) {
        shader()->SetUniform("Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE9);
        shader()->SetUniform("Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE10);
    }
}

void Material::bind_values()
{
    for (const auto extension : GetComponents<MaterialExtension>()) {
        for (const auto stage : extension->ShaderExtension()->Stages()) {
            for (const auto )
        }
    }
}

std::shared_ptr<Shader> Material::shader()
{
    return _shader.lock();
}

std::shared_ptr<Shader> Material::depth_shader()
{
    return _depth_shader.lock();
}

std::shared_ptr<Texture2D> Material::TextureEmissive()
{
    return _texture_emitting;
}

std::shared_ptr<Texture2D> Material::TextureNormal()
{
    return _texture_normal;
}

std::shared_ptr<Texture2D> Material::TextureHeight()
{
    return _texture_height;
}

std::shared_ptr<Texture2D> Material::TextureAO()
{
    return _texture_ao;
}

void Material::SetTextureEmissive(std::shared_ptr<Texture2D> t)
{
    _texture_emitting = t;
    shader()->SetUniform("StandardTextures.Emissive", TextureEmissive(), GL_TEXTURE5);
    TextureEmissive() ? shader()->SetDefine("TEXTURE_USE_EMITTING") : shader()->RemoveDefine("TEXTURE_USE_EMITTING");
}

void Material::SetTextureNormal(std::shared_ptr<Texture2D> t)
{
    _texture_normal = t;
    shader()->SetUniform("StandardTextures.Normal", TextureNormal(), GL_TEXTURE6);
    TextureNormal() ? shader()->SetDefine("TEXTURE_USE_NORMAL") : shader()->RemoveDefine("TEXTURE_USE_NORMAL");
}

void Material::SetTextureHeight(std::shared_ptr<Texture2D> t)
{
    _texture_height = t;
    shader()->SetUniform("StandardTextures.Height", TextureHeight(), GL_TEXTURE7);
    TextureHeight() ? shader()->SetDefine("TEXTURE_USE_HEIGHT") : shader()->RemoveDefine("TEXTURE_USE_HEIGHT");
}

void Material::SetTextureAO(std::shared_ptr<Texture2D> t)
{
    _texture_ao = t;
    shader()->SetUniform("StandardTextures.AO", TextureAO(), GL_TEXTURE8);
    TextureHeight() ? shader()->SetDefine("TEXTURE_USE_AO") : shader()->RemoveDefine("TEXTURE_USE_AO");
}

glm::vec3 Material::Emissive() const
{
    return _emissive;
}

void Material::SetEmissive(glm::vec3 value)
{
    shader()->SetUniform("_StandardValues.Emissive", value);
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
    shader()->SetUniform("_StandardValues.Opacity", value);
    _opacity = value;
}

float Material::Parallax() const
{
    return _parallax;
}

void Material::SetParallax(float value)
{
    shader()->SetUniform("_StandardValues.Parallax", value);
    _parallax = value;
}

float Material::Ior() const
{
    return _ior;
}

void Material::SetIor(float value)
{
    shader()->SetUniform("_StandardValues.Ior", value);
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
