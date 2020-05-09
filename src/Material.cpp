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
    _shader = Shader::Create(Name() + "_shader", ForwardShader);
    _shader.lock()->Stage(GL_FRAGMENT_SHADER).SetTechnique(forward_default_frag_technique);
    _depth_shader = Shader::Create(Name() + "_depth_shader", ForwardShader);
    _depth_shader.lock()->SetStage(ShaderStage(GL_VERTEX_SHADER, depth_vert_code));
    _depth_shader.lock()->SetStage(ShaderStage(GL_FRAGMENT_SHADER, depth_frag_code));
    shader()->SetUniform("Material.Albedo", Albedo());
    shader()->SetUniform("Material.Specular", Specular());
    shader()->SetUniform("Material.Emitting", Emitting());
    shader()->SetUniform("Texture.Scale", UVScale());
    shader()->SetUniform("Material.Roughness", Roughness());
    shader()->SetUniform("Material.Metallic", Metallic());
    shader()->SetUniform("Material.Alpha", Alpha());
    shader()->SetUniform("Material.Parallax", Parallax());
    shader()->SetUniform("Material.Ior", Ior());
    depth_shader()->SetUniform("Material.Albedo", Albedo());
    depth_shader()->SetUniform("Material.Specular", Specular());
    depth_shader()->SetUniform("Material.Emitting", Emitting());
    depth_shader()->SetUniform("Texture.Scale", UVScale());
    depth_shader()->SetUniform("Material.Roughness", Roughness());
    depth_shader()->SetUniform("Material.Metallic", Metallic());
    depth_shader()->SetUniform("Material.Alpha", Alpha());
    depth_shader()->SetUniform("Material.Parallax", Parallax());
    depth_shader()->SetUniform("Material.Ior", Ior());
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
    if (TextureMetallicRoughness() != nullptr) {
        shader()->SetDefine("TEXTURE_USE_METALLICROUGHNESS");
        shader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        shader()->RemoveDefine("TEXTURE_USE_METALLIC");
    }
    else {
        shader()->RemoveDefine("TEXTURE_USE_METALLICROUGHNESS");
        TextureRoughness() ? shader()->SetDefine("TEXTURE_USE_ROUGHNESS") : shader()->RemoveDefine("TEXTURE_USE_ROUGHNESS");
        TextureMetallic() ? shader()->SetDefine("TEXTURE_USE_METALLIC") : shader()->RemoveDefine("TEXTURE_USE_METALLIC");
    }
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
    
}

std::shared_ptr<Shader> Material::shader()
{
    return _shader.lock();
}

std::shared_ptr<Shader> Material::depth_shader()
{
    return _depth_shader.lock();
}

std::shared_ptr<Texture2D> Material::TextureAlbedo()
{
    return _texture_albedo;
}

std::shared_ptr<Texture2D> Material::TextureSpecular()
{
    return _texture_specular;
}

std::shared_ptr<Texture2D> Material::TextureEmitting()
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

std::shared_ptr<Texture2D> Material::TextureMetallicRoughness()
{
    return _texture_metallicRoughness;
}

std::shared_ptr<Texture2D> Material::TextureRoughness()
{
    return _texture_roughness;
}

std::shared_ptr<Texture2D> Material::TextureMetallic()
{
    return _texture_metallic;
}

std::shared_ptr<Texture2D> Material::TextureAO()
{
    return _texture_ao;
}

void Material::SetTextureAlbedo(std::shared_ptr<Texture2D> t)
{
    _texture_albedo = t;
    shader()->SetUniform("Texture.Albedo", TextureAlbedo(), GL_TEXTURE0);
    TextureAlbedo() ? shader()->SetDefine("TEXTURE_USE_ALBEDO") : shader()->RemoveDefine("TEXTURE_USE_ALBEDO");
}

void Material::SetTextureSpecular(std::shared_ptr<Texture2D> t)
{
    _texture_specular = t;
    shader()->SetUniform("Texture.Specular", TextureSpecular(), GL_TEXTURE1);
    TextureSpecular() ? shader()->SetDefine("TEXTURE_USE_SPECULAR") : shader()->RemoveDefine("TEXTURE_USE_SPECULAR");
}

void Material::SetTextureEmitting(std::shared_ptr<Texture2D> t)
{
    _texture_emitting = t;
    shader()->SetUniform("Texture.Emitting", TextureEmitting(), GL_TEXTURE5);
    TextureEmitting() ? shader()->SetDefine("TEXTURE_USE_EMITTING") : shader()->RemoveDefine("TEXTURE_USE_EMITTING");
}

void Material::SetTextureNormal(std::shared_ptr<Texture2D> t)
{
    _texture_normal = t;
    shader()->SetUniform("Texture.Normal", TextureNormal(), GL_TEXTURE6);
    TextureNormal() ? shader()->SetDefine("TEXTURE_USE_NORMAL") : shader()->RemoveDefine("TEXTURE_USE_NORMAL");
}

void Material::SetTextureHeight(std::shared_ptr<Texture2D> t)
{
    _texture_height = t;
    shader()->SetUniform("Texture.Height", TextureHeight(), GL_TEXTURE7);
    TextureHeight() ? shader()->SetDefine("TEXTURE_USE_HEIGHT") : shader()->RemoveDefine("TEXTURE_USE_HEIGHT");
}

void Material::SetTextureMetallicRoughness(std::shared_ptr<Texture2D> t)
{
    _texture_metallicRoughness = t;
    shader()->SetUniform("Texture.MetallicRoughness", TextureMetallicRoughness(), GL_TEXTURE2);
}

void Material::SetTextureRoughness(std::shared_ptr<Texture2D> t)
{
    _texture_roughness = t;
    shader()->SetUniform("Texture.Roughness", TextureRoughness(), GL_TEXTURE3);
}

void Material::SetTextureMetallic(std::shared_ptr<Texture2D> t)
{
    _texture_metallic = t;
    shader()->SetUniform("Texture.Metallic", TextureMetallic(), GL_TEXTURE4);
}

void Material::SetTextureAO(std::shared_ptr<Texture2D> t)
{
    _texture_ao = t;
    shader()->SetUniform("Texture.AO", TextureAO(), GL_TEXTURE8);
    TextureHeight() ? shader()->SetDefine("TEXTURE_USE_AO") : shader()->RemoveDefine("TEXTURE_USE_AO");
}

glm::vec3 Material::Albedo() const
{
    return _albedo;
}

void Material::SetAlbedo(glm::vec3 value)
{
    shader()->SetUniform("Material.Albedo", value);
    _albedo = value;
}

glm::vec3 Material::Specular() const
{
    return _specular;
}

void Material::SetSpecular(glm::vec3 value)
{
    shader()->SetUniform("Material.Specular", value);
    _specular = value;
}

glm::vec3 Material::Emitting() const
{
    return _emitting;
}

void Material::SetEmitting(glm::vec3 value)
{
    shader()->SetUniform("Material.Emitting", value);
    _emitting = value;
}

glm::vec2 Material::UVScale() const
{
    return _uv_scale;
}

void Material::SetUVScale(glm::vec2 value)
{
    shader()->SetUniform("Texture.Scale", value);
    _uv_scale = value;
}

float Material::Roughness() const
{
    return _roughness;
}

void Material::SetRoughness(float value)
{
    shader()->SetUniform("Material.Roughness", value);
    _roughness = value;
}

float Material::Metallic() const
{
    return _metallic;
}

void Material::SetMetallic(float value)
{
    shader()->SetUniform("Material.Metallic", value);
    _metallic = value;
}

float Material::Alpha() const
{
    return _alpha;
}

void Material::SetAlpha(float value)
{
    shader()->SetUniform("Material.Alpha", value);
    _alpha = value;
}

float Material::Parallax() const
{
    return _parallax;
}

void Material::SetParallax(float value)
{
    shader()->SetUniform("Material.Parallax", value);
    _parallax = value;
}

float Material::Ior() const
{
    return _ior;
}

void Material::SetIor(float value)
{
    shader()->SetUniform("Material.Ior", value);
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
