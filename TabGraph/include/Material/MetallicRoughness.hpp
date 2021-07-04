/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 22:49:43
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Material/Extensions/Extension.hpp>
#include <Core/Inherit.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture2D;
}

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Material::Extensions {
class MetallicRoughness : public Core::Inherit<Extension, MetallicRoughness> {
public:
    MetallicRoughness();
    std::shared_ptr<Textures::Texture2D> GetTextureBaseColor();
    std::shared_ptr<Textures::Texture2D> GetTextureMetallicRoughness();
    std::shared_ptr<Textures::Texture2D> GetTextureRoughness();
    std::shared_ptr<Textures::Texture2D> GetTextureMetallic();
    float GetRoughness() const;
    float GetMetallic() const;
    float GetOpacity() const;
    glm::vec3 GetBaseColor() const;
    void SetTextureBaseColor(std::shared_ptr<Textures::Texture2D>);
    void SetTextureMetallicRoughness(std::shared_ptr<Textures::Texture2D>);
    void SetTextureRoughness(std::shared_ptr<Textures::Texture2D>);
    void SetTextureMetallic(std::shared_ptr<Textures::Texture2D>);
    void SetRoughness(float);
    void SetMetallic(float);
    void SetOpacity(float);
    void SetBaseColor(glm::vec3);
};
}

