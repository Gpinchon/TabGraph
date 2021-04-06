/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-27 11:38:37
*/

#pragma once

#include "Component.hpp" // for Component
#include "Material/MaterialExtension.hpp"
#include "Renderer/Renderer.hpp"

#include "Property.hpp" // for READONLYPROPERTY, PRIVATEPROPERTY, PROPERTY
#include "glm/ext/vector_float2.hpp" // for vec2
#include "glm/ext/vector_float3.hpp" // for vec3
#include "xstring" // for string
#include <array> // for array
#include <memory> // for shared_ptr, weak_ptr

namespace Shader {
class Program;
};
class Texture2D;
class MaterialExtension;

class Material : public MaterialExtension {
public:
    enum class OpacityMode {
        Opaque,
        Mask,
        Blend,
        MaxValue
    };
    PROPERTY(bool, DoubleSided, false);
    PROPERTY(OpacityMode, OpacityMode, OpacityMode::Opaque);
    PROPERTY(glm::vec2, UVScale, 1);

public:
    Material(const std::string& name);
    virtual void AddExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual void RemoveExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual std::shared_ptr<MaterialExtension> GetExtension(const std::string& name) const final;
    virtual void Bind(const Renderer::Options::Pass& pass);
    std::shared_ptr<Shader::Program> GetShader(const Renderer::Options::Pass& pass);
    void SetShader(const Renderer::Options::Pass& pass, std::shared_ptr<Shader::Program> shader);

    glm::vec3 GetDiffuse(void);
    glm::vec3 GetEmissive(void);
    float GetOpacityCutoff(void);
    float GetOpacity(void);
    float GetParallax(void);
    float GetIor(void);
    std::shared_ptr<Texture2D> GetTextureDiffuse(void);
    std::shared_ptr<Texture2D> GetTextureEmissive(void);
    std::shared_ptr<Texture2D> GetTextureNormal(void);
    std::shared_ptr<Texture2D> GetTextureHeight(void);
    std::shared_ptr<Texture2D> GetTextureAO(void);
    std::shared_ptr<Texture2D> GetTextureBRDFLUT(void);

    void SetDiffuse(glm::vec3 value);
    void SetEmissive(glm::vec3 value);
    void SetOpacityCutoff(float value);
    void SetOpacity(float value);
    void SetParallax(float value);
    void SetIor(float value);
    void SetTextureDiffuse(std::shared_ptr<Texture2D>);
    void SetTextureEmissive(std::shared_ptr<Texture2D>);
    void SetTextureNormal(std::shared_ptr<Texture2D>);
    void SetTextureHeight(std::shared_ptr<Texture2D>);
    void SetTextureAO(std::shared_ptr<Texture2D>);
    void SetTextureBRDFLUT(std::shared_ptr<Texture2D>);

protected:
    PRIVATEPROPERTY(bool, ShaderChanged, false);
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto clone = Component::Create<Material>(*this);
        return clone;
    }
    std::array<std::shared_ptr<Shader::Program>, (int)Renderer::Options::Pass::MaxValue> _shaders;
};