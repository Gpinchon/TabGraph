/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:04
*/

#include "Material.hpp"
#include "Cubemap.hpp"
#include "Environment.hpp" // for Environment
#include "Shader.hpp" // for Shader
#include "parser/GLSL.hpp" // for GLSL, ForwardShader
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
    : Object(name)
{
    _shader = Shader::Create("default_shader", ForwardShader);
    _shader.lock()->Stage(GL_FRAGMENT_SHADER).SetTechnique(forward_default_frag_technique);
    _depth_shader = Shader::Create("default_shader", ForwardShader);
    _depth_shader.lock()->SetStage(ShaderStage(GL_VERTEX_SHADER, depth_vert_code));
    _depth_shader.lock()->SetStage(ShaderStage(GL_FRAGMENT_SHADER, depth_frag_code));
}

std::shared_ptr<Material> Material::Create(const std::string& name)
{
    auto mtl = std::shared_ptr<Material>(new Material(name));
    return mtl;
}

void Material::Bind()
{
    if (nullptr == shader())
        return;
    texture_albedo() ? shader()->SetDefine("TEXTURE_USE_ALBEDO") : shader()->RemoveDefine("TEXTURE_USE_ALBEDO");
    texture_specular() ? shader()->SetDefine("TEXTURE_USE_SPECULAR") : shader()->RemoveDefine("TEXTURE_USE_SPECULAR");
    texture_emitting() ? shader()->SetDefine("TEXTURE_USE_EMITTING") : shader()->RemoveDefine("TEXTURE_USE_EMITTING");
    texture_normal() ? shader()->SetDefine("TEXTURE_USE_NORMAL") : shader()->RemoveDefine("TEXTURE_USE_NORMAL");
    texture_height() ? shader()->SetDefine("TEXTURE_USE_HEIGHT") : shader()->RemoveDefine("TEXTURE_USE_HEIGHT");
    if (texture_metallicRoughness() != nullptr) {
        shader()->SetDefine("TEXTURE_USE_METALLICROUGHNESS");
        shader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        shader()->RemoveDefine("TEXTURE_USE_METALLIC");
    }
    else {
        shader()->RemoveDefine("TEXTURE_USE_METALLICROUGHNESS");
        texture_roughness() ? shader()->SetDefine("TEXTURE_USE_ROUGHNESS") : shader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        texture_metallic() ? shader()->SetDefine("TEXTURE_USE_METALLIC") : shader()->RemoveDefine("TEXTURE_USE_METALLIC");
    }
    shader()->use();
    bind_textures();
    bind_values();
    shader()->use(false);
}

void Material::bind_textures()
{
    shader()->bind_texture("Texture.Albedo", texture_albedo(), GL_TEXTURE0);
    shader()->bind_texture("Texture.Specular", texture_specular(), GL_TEXTURE1);
    if (texture_metallicRoughness() != nullptr)
        shader()->bind_texture("Texture.MetallicRoughness", texture_metallicRoughness(), GL_TEXTURE2);
    else {
        shader()->bind_texture("Texture.Roughness", texture_roughness(), GL_TEXTURE3);
        shader()->bind_texture("Texture.Metallic", texture_metallic(), GL_TEXTURE4);
    }
    shader()->bind_texture("Texture.Emitting", texture_emitting(), GL_TEXTURE5);
    shader()->bind_texture("Texture.Normal", texture_normal(), GL_TEXTURE6);
    shader()->bind_texture("Texture.Height", texture_height(), GL_TEXTURE7);
    shader()->bind_texture("Texture.AO", texture_ao(), GL_TEXTURE8);
    if (Environment::current() != nullptr) {
        shader()->bind_texture("Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE9);
        shader()->bind_texture("Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE10);
    }
}

void Material::bind_values()
{
    shader()->SetUniform("Material.Albedo", albedo);
    shader()->SetUniform("Material.Specular", specular);
    shader()->SetUniform("Material.Emitting", emitting);
    shader()->SetUniform("Material.Roughness", roughness);
    shader()->SetUniform("Material.Metallic", metallic);
    shader()->SetUniform("Material.Alpha", alpha);
    shader()->SetUniform("Material.Parallax", parallax);
    shader()->SetUniform("Material.Ior", ior);
    shader()->SetUniform("Texture.Scale", uv_scale);
}

std::shared_ptr<Shader> Material::shader()
{
    return _shader.lock();
}

std::shared_ptr<Shader> Material::depth_shader()
{
    return _depth_shader.lock();
}

std::shared_ptr<Texture2D> Material::texture_albedo()
{
    return _texture_albedo;
}

std::shared_ptr<Texture2D> Material::texture_specular()
{
    return _texture_specular;
}

std::shared_ptr<Texture2D> Material::texture_emitting()
{
    return _texture_emitting;
}

std::shared_ptr<Texture2D> Material::texture_normal()
{
    return _texture_normal;
}

std::shared_ptr<Texture2D> Material::texture_height()
{
    return _texture_height;
}

std::shared_ptr<Texture2D> Material::texture_metallicRoughness()
{
    return _texture_metallicRoughness;
}

std::shared_ptr<Texture2D> Material::texture_roughness()
{
    return _texture_roughness;
}

std::shared_ptr<Texture2D> Material::texture_metallic()
{
    return _texture_metallic;
}

std::shared_ptr<Texture2D> Material::texture_ao()
{
    return _texture_ao;
}

void Material::set_texture_albedo(std::shared_ptr<Texture2D> t)
{
    _texture_albedo = t;
}

void Material::set_texture_specular(std::shared_ptr<Texture2D> t)
{
    _texture_specular = t;
}

void Material::set_texture_emitting(std::shared_ptr<Texture2D> t)
{
    _texture_emitting = t;
}

void Material::set_texture_normal(std::shared_ptr<Texture2D> t)
{
    _texture_normal = t;
}

void Material::set_texture_height(std::shared_ptr<Texture2D> t)
{
    _texture_height = t;
}

void Material::set_texture_metallicRoughness(std::shared_ptr<Texture2D> t)
{
    _texture_metallicRoughness = t;
}

void Material::set_texture_roughness(std::shared_ptr<Texture2D> t)
{
    _texture_roughness = t;
}

void Material::set_texture_metallic(std::shared_ptr<Texture2D> t)
{
    _texture_metallic = t;
}

void Material::set_texture_ao(std::shared_ptr<Texture2D> t)
{
    _texture_ao = t;
}
