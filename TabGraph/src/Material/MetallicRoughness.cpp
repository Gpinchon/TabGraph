#include "..\..\include\Material\MetallicRoughness.hpp"

std::shared_ptr<MetallicRoughness> MetallicRoughness::Create()
{
    auto ptr = std::shared_ptr<MetallicRoughness>();
    ptr->SetName("MetallicRoughness");
    return ptr;
}

std::shared_ptr<Texture2D> MetallicRoughness::TextureBaseColor() const
{
    return _texture_baseColor;
}

std::shared_ptr<Texture2D> MetallicRoughness::TextureMetallicRoughness() const
{
    return _texture_metallicRoughness;
}

std::shared_ptr<Texture2D> MetallicRoughness::TextureRoughness() const
{
    return _texture_roughness;
}

std::shared_ptr<Texture2D> MetallicRoughness::TextureMetallic() const
{
    return _texture_metallic;
}

void MetallicRoughness::SetTextureBaseColor(std::shared_ptr<Texture2D> texture)
{
    _texture_baseColor = texture;
}

void MetallicRoughness::SetTextureMetallicRoughness(std::shared_ptr<Texture2D> texture)
{
    _texture_metallicRoughness = texture;
}

void MetallicRoughness::SetTextureRoughness(std::shared_ptr<Texture2D> texture)
{
    _texture_roughness = texture;
}

void MetallicRoughness::SetTextureMetallic(std::shared_ptr<Texture2D> texture)
{
    _texture_metallic = texture;
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
    _roughness = value;
}

void MetallicRoughness::SetMetallic(float value)
{
    _metallic = value;
}

void MetallicRoughness::SetBaseColor(glm::vec4 value)
{
    _baseColor = value;
}
