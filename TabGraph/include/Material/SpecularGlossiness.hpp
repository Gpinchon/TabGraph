/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-22 20:46:27
*/
#pragma once
#include "MaterialExtension.hpp"

#include <glm/vec4.hpp>

class SpecularGlossiness : public MaterialExtension {
public:
    SpecularGlossiness();
    std::shared_ptr<Texture2D> GetTextureDiffuse() const;
    std::shared_ptr<Texture2D> GetTextureSpecular() const;
    std::shared_ptr<Texture2D> GetTextureGlossiness() const;
    std::shared_ptr<Texture2D> GetTextureSpecularGlossiness() const;
    glm::vec3 GetDiffuse() const;
    glm::vec3 GetSpecular() const;
    float GetGlossines() const;
    float GetOpacity() const;
    void SetTextureDiffuse(std::shared_ptr<Texture2D> texture);
    void SetTextureSpecular(std::shared_ptr<Texture2D> texture);
    void SetTextureGlossiness(std::shared_ptr<Texture2D> texture);
    void SetTextureSpecularGlossiness(std::shared_ptr<Texture2D> texture);
    void SetDiffuse(glm::vec3);
    void SetSpecular(glm::vec3);
    void SetGlossiness(float);
    void SetOpacity(float);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<SpecularGlossiness>(*this);
    }
};
