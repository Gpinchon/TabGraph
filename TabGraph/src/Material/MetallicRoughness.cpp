#include "Material/MetallicRoughness.hpp"
#include "Texture/Texture2D.hpp"

MetallicRoughness::MetallicRoughness() : MaterialExtension("MetallicRoughness")
{
    static auto extensionCode =
#include "metallicRoughness.frag"
    ;
    SetCode({ extensionCode, "MetallicRoughness();" });
}

std::shared_ptr<Texture2D> MetallicRoughness::GetTextureBaseColor()
{
    return GetTexture("MetallicRoughnessTextures.BaseColor");
}

std::shared_ptr<Texture2D> MetallicRoughness::GetTextureMetallicRoughness()
{
    return GetTexture("MetallicRoughnessTextures.MetallicRoughness");
}

std::shared_ptr<Texture2D> MetallicRoughness::GetTextureRoughness()
{
    return GetTexture("MetallicRoughnessTextures.Roughness");
}

std::shared_ptr<Texture2D> MetallicRoughness::GetTextureMetallic()
{
    return GetTexture("MetallicRoughnessTextures.Metallic");
}

void MetallicRoughness::SetTextureBaseColor(std::shared_ptr<Texture2D> texture)
{
    SetTexture("MetallicRoughnessTextures.BaseColor", texture);
    texture ? SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR") : RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR");
}

void MetallicRoughness::SetTextureMetallicRoughness(std::shared_ptr<Texture2D> texture)
{
    SetTexture("MetallicRoughnessTextures.MetallicRoughness", texture);
    texture ? SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS") : RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS");
}

void MetallicRoughness::SetTextureRoughness(std::shared_ptr<Texture2D> texture)
{
    SetTexture("MetallicRoughnessTextures.Roughness", texture);
    texture ? SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS") : RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS");
}

void MetallicRoughness::SetTextureMetallic(std::shared_ptr<Texture2D> texture)
{
    SetTexture("MetallicRoughnessTextures.Metallic", texture);
    texture ? SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC") : RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC");
}

void MetallicRoughness::SetRoughness(float value)
{
    SetValue("MetallicRoughnessRoughness", value);
}

void MetallicRoughness::SetMetallic(float value)
{
    SetValue("MetallicRoughnessMetallic", value);
}

void MetallicRoughness::SetOpacity(float value)
{
    SetValue("MetallicRoughnessOpacity", value);
}

void MetallicRoughness::SetBaseColor(glm::vec3 value)
{
    SetColor("MetallicRoughnessBaseColor", value);
}

float MetallicRoughness::GetRoughness() const
{
    return GetValue("MetallicRoughnessRoughness");
}

float MetallicRoughness::GetMetallic() const
{
    return GetValue("MetallicRoughnessMetallic");
}

float MetallicRoughness::GetOpacity() const
{
    return GetValue("MetallicRoughnessOpacity");
}

glm::vec3 MetallicRoughness::GetBaseColor() const
{
    return GetColor("MetallicRoughnessBaseColor");
}
