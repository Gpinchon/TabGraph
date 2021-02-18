#include "Material/MetallicRoughness.hpp"
#include "Shader/Shader.hpp"
#include "Texture/Texture2D.hpp"

MetallicRoughness::MetallicRoughness() : MaterialExtension("MetallicRoughness")
{
    static auto extensionCode =
#include "metallicRoughness.frag"
    ;
    GetShaderExtension()->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(extensionCode, "MetallicRoughness();")));
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.BaseColor", GetBaseColor());
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.Metallic", GetMetallic());
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.Roughness", GetRoughness());
}

void MetallicRoughness::SetTextureBaseColor(std::shared_ptr<Texture2D> texture)
{
    GetShaderExtension()->SetTexture("MetallicRoughnessTextures.BaseColor", texture);
    texture ? GetShaderExtension()->SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR") : GetShaderExtension()->RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR");
    _SetTextureBaseColor(texture);
}

void MetallicRoughness::SetTextureMetallicRoughness(std::shared_ptr<Texture2D> texture)
{
    GetShaderExtension()->SetTexture("MetallicRoughnessTextures.MetallicRoughness", texture);
    texture ? GetShaderExtension()->SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS") : GetShaderExtension()->RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS");
    _SetTextureMetallicRoughness(texture);
}

void MetallicRoughness::SetTextureRoughness(std::shared_ptr<Texture2D> texture)
{
    GetShaderExtension()->SetTexture("MetallicRoughnessTextures.Roughness", texture);
    texture ? GetShaderExtension()->SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS") : GetShaderExtension()->RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS");
    _SetTextureRoughness(texture);
}

void MetallicRoughness::SetTextureMetallic(std::shared_ptr<Texture2D> texture)
{
    GetShaderExtension()->SetTexture("MetallicRoughnessTextures.Metallic", texture);
    texture ? GetShaderExtension()->SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC") : GetShaderExtension()->RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC");
    _SetTextureMetallic(texture);
}

void MetallicRoughness::SetRoughness(float value)
{
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.Roughness", value);
    _SetRoughness(value);
}

void MetallicRoughness::SetMetallic(float value)
{
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.Metallic", value);
    _SetMetallic(value);
}

void MetallicRoughness::SetBaseColor(glm::vec4 value)
{
    GetShaderExtension()->SetUniform("MetallicRoughnessValues.BaseColor", value);
    _SetBaseColor(value);
}
