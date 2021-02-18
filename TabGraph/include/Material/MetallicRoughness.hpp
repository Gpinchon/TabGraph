/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 22:49:43
*/
#pragma once
#include "MaterialExtension.hpp"

#include <glm/vec4.hpp>

class MetallicRoughness : public MaterialExtension {
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureBaseColor, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureMetallicRoughness, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureRoughness, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureMetallic, nullptr);
    READONLYPROPERTY(float, Roughness, 1);
    READONLYPROPERTY(float, Metallic, 1);
    READONLYPROPERTY(glm::vec4, BaseColor, 1);

public:
    MetallicRoughness();
    void SetTextureBaseColor(std::shared_ptr<Texture2D>);
    void SetTextureMetallicRoughness(std::shared_ptr<Texture2D>);
    void SetTextureRoughness(std::shared_ptr<Texture2D>);
    void SetTextureMetallic(std::shared_ptr<Texture2D>);
    void SetRoughness(float);
    void SetMetallic(float);
    void SetBaseColor(glm::vec4);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<MetallicRoughness>(*this);
    }
};
