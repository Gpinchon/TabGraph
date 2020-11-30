/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-10-22 12:56:36
*/
#pragma once
#include "MaterialExtension.hpp"

#include <glm/vec4.hpp>

class SpecularGlossiness : public MaterialExtension {
public:
    SpecularGlossiness();
    std::shared_ptr<Texture2D> TextureDiffuse() const;
    std::shared_ptr<Texture2D> TextureSpecular() const;
    std::shared_ptr<Texture2D> TextureGlossiness() const;
    std::shared_ptr<Texture2D> TextureSpecularGlossiness() const;
    void SetTextureDiffuse(std::shared_ptr<Texture2D>);
    void SetTextureSpecular(std::shared_ptr<Texture2D>);
    void SetTextureGlossiness(std::shared_ptr<Texture2D>);
    void SetTextureSpecularGlossiness(std::shared_ptr<Texture2D>);
    glm::vec4 Diffuse() const;
    glm::vec3 Specular() const;
    float Glossiness() const;
    void SetDiffuse(glm::vec4);
    void SetSpecular(glm::vec3);
    void SetGlossiness(float);

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<SpecularGlossiness>(*this);
    }
    int64_t _texture_diffuse{ -1 };
    int64_t _texture_specular{ -1 };
    int64_t _texture_glossiness{ -1 };
    int64_t _texture_specularGlossiness{ -1 };
    glm::vec4 _diffuse { 1, 1, 1, 1 };
    glm::vec3 _specular { 1, 1, 1 };
    float _glossiness { 1 };
};
