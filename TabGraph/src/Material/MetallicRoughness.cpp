#include "Material/MetallicRoughness.hpp"
#include "Shader/Shader.hpp"
#include "Texture/Texture2D.hpp"

MetallicRoughness::MetallicRoughness() : MaterialExtension("MetallicRoughness")
{
    static auto extensionCode =
#include "metallicRoughness.frag"
    ;
    GetShaderExtension()->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(extensionCode, "MetallicRoughness();")));
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.BaseColor", BaseColor());
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.Metallic", Metallic());
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.Roughness", Roughness());
}

std::shared_ptr<Texture2D> MetallicRoughness::TextureBaseColor() const
{
    return GetComponent<Texture2D>(_texture_baseColor);
}

std::shared_ptr<Texture2D> MetallicRoughness::TextureMetallicRoughness() const
{
    return GetComponent<Texture2D>(_texture_metallicRoughness);
}

std::shared_ptr<Texture2D> MetallicRoughness::TextureRoughness() const
{
    return GetComponent<Texture2D>(_texture_roughness);
}

std::shared_ptr<Texture2D> MetallicRoughness::TextureMetallic() const
{
    return GetComponent<Texture2D>(_texture_metallic);
}

void MetallicRoughness::SetTextureBaseColor(std::shared_ptr<Texture2D> texture)
{
    _texture_baseColor = AddComponent(texture);
    GetShaderExtension()->SetTexture("MetallicRoughnessTextures.BaseColor", texture);
    texture ? GetShaderExtension()->SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR") : GetShaderExtension()->RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR");
}

void MetallicRoughness::SetTextureMetallicRoughness(std::shared_ptr<Texture2D> texture)
{
    _texture_metallicRoughness = AddComponent(texture);
    GetShaderExtension()->SetTexture("MetallicRoughnessTextures.MetallicRoughness", texture);
    texture ? GetShaderExtension()->SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS") : GetShaderExtension()->RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS");
}

void MetallicRoughness::SetTextureRoughness(std::shared_ptr<Texture2D> texture)
{
    _texture_roughness = AddComponent(texture);
    GetShaderExtension()->SetTexture("MetallicRoughnessTextures.Roughness", texture);
    texture ? GetShaderExtension()->SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS") : GetShaderExtension()->RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS");
}

void MetallicRoughness::SetTextureMetallic(std::shared_ptr<Texture2D> texture)
{
    _texture_metallic = AddComponent(texture);
    GetShaderExtension()->SetTexture("MetallicRoughnessTextures.Metallic", texture);
    texture ? GetShaderExtension()->SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC") : GetShaderExtension()->RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC");
}

float MetallicRoughness::Roughness() const
{
    return _roughness;
}

float MetallicRoughness::Metallic() const
{
    return _metallic;
}

glm::vec4 MetallicRoughness::BaseColor() const
{
    return _baseColor;
}

void MetallicRoughness::SetRoughness(float value)
{
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.Roughness", value);
    _roughness = value;
}

void MetallicRoughness::SetMetallic(float value)
{
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.Metallic", value);
    _metallic = value;
}

void MetallicRoughness::SetBaseColor(glm::vec4 value)
{
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.BaseColor", value);
    _baseColor = value;
}
