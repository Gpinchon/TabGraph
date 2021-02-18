/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 23:02:52
*/
#pragma once
#include "MaterialExtension.hpp"

#include <glm/vec4.hpp>

class SpecularGlossiness : public MaterialExtension {
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureDiffuse, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureSpecular, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureGlossiness, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureSpecularGlossiness, nullptr);
    READONLYPROPERTY(glm::vec4, Diffuse, 1);
    READONLYPROPERTY(glm::vec3, Specular, 1);
    READONLYPROPERTY(float, Glossiness, 1);

public:
    SpecularGlossiness();
    void SetTextureDiffuse(std::shared_ptr<Texture2D> texture);
    void SetTextureSpecular(std::shared_ptr<Texture2D> texture);
    void SetTextureGlossiness(std::shared_ptr<Texture2D> texture);
    void SetTextureSpecularGlossiness(std::shared_ptr<Texture2D> texture);
    void SetDiffuse(glm::vec4);
    void SetSpecular(glm::vec3);
    void SetGlossiness(float);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<SpecularGlossiness>(*this);
    }
};
