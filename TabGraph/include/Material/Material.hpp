/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 22:12:30
*/

#pragma once

#include "Component.hpp" // for Component

#include <glm/glm.hpp> // for glm::vec3, glm::vec2
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Shader;
class Texture2D;
class MaterialExtension;

class Material : public Component {
public:
    enum class OpacityModeValue {
        Opaque,
        Mask,
        Blend
    };
    PROPERTY(bool, DoubleSided, false);
    READONLYPROPERTY(OpacityModeValue, OpacityMode, OpacityModeValue::Opaque);
    READONLYPROPERTY(float, OpacityCutoff, 0.5);
    READONLYPROPERTY(glm::vec3, Diffuse, 1);
    READONLYPROPERTY(glm::vec3, Emissive, 0);
    READONLYPROPERTY(glm::vec2, UVScale, 1);
    READONLYPROPERTY(float, Opacity, 1);
    READONLYPROPERTY(float, Parallax, 0.01);
    READONLYPROPERTY(float, Ior, 1);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureDiffuse, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureEmissive, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureNormal, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureHeight, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureAO, nullptr);
    READONLYPROPERTY(std::shared_ptr<Texture2D>, TextureBRDFLUT, nullptr);
    READONLYPROPERTY(std::shared_ptr<Shader>, MaterialShader, nullptr);
    READONLYPROPERTY(std::shared_ptr<Shader>, GeometryShader, nullptr);

public:
    Material(const std::string& name);
    virtual void AddExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual void RemoveExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual std::shared_ptr<MaterialExtension> GetExtension(const std::string& name) const final;
    virtual void Bind();
    void SetGeometryShader(std::shared_ptr<Shader> shader);
    void SetMaterialShader(std::shared_ptr<Shader> shader);
    void SetOpacityMode(OpacityModeValue value);
    void SetOpacityCutoff(float value);
    void SetDiffuse(glm::vec3 value);
    void SetEmissive(glm::vec3 value);
    void SetUVScale(glm::vec2 value);
    void SetOpacity(float value);
    void SetParallax(float value);
    void SetIor(float value);
    void SetTextureDiffuse(std::shared_ptr<Texture2D>);
    void SetTextureEmissive(std::shared_ptr<Texture2D>);
    void SetTextureNormal(std::shared_ptr<Texture2D>);
    void SetTextureHeight(std::shared_ptr<Texture2D>);
    void SetTextureAO(std::shared_ptr<Texture2D>);
    void SetTextureBRDFLUT(const std::shared_ptr<Texture2D>&);

protected:
    PRIVATEPROPERTY(bool, ShaderChanged, false);
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto clone = Component::Create<Material>(*this);
        return clone;
    }
    void bind_values();
    void bind_textures();
};