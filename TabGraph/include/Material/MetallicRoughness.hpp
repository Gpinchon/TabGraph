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
public:
    MetallicRoughness();
    std::shared_ptr<Texture2D> GetTextureBaseColor();
    std::shared_ptr<Texture2D> GetTextureMetallicRoughness();
    std::shared_ptr<Texture2D> GetTextureRoughness();
    std::shared_ptr<Texture2D> GetTextureMetallic();
    float GetRoughness() const;
    float GetMetallic() const;
    float GetOpacity() const;
    glm::vec3 GetBaseColor() const;
    void SetTextureBaseColor(std::shared_ptr<Texture2D>);
    void SetTextureMetallicRoughness(std::shared_ptr<Texture2D>);
    void SetTextureRoughness(std::shared_ptr<Texture2D>);
    void SetTextureMetallic(std::shared_ptr<Texture2D>);
    void SetRoughness(float);
    void SetMetallic(float);
    void SetOpacity(float);
    void SetBaseColor(glm::vec3);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<MetallicRoughness>(*this);
    }
};
