/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-19 00:32:02
*/

#pragma once

#include "Component.hpp" // for Component
#include "Render.hpp"

#include <memory> // for shared_ptr, weak_ptr
#include <array> // for array
#include "glm/ext/vector_float2.hpp"  // for vec2
#include "glm/ext/vector_float3.hpp"  // for vec3
#include "Property.hpp"               // for READONLYPROPERTY, PRIVATEPROPERTY, PROPERTY
#include "xstring"                    // for string

namespace Shader {
    class Program;
};
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

public:
    Material(const std::string& name);
    virtual void AddExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual void RemoveExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual std::shared_ptr<MaterialExtension> GetExtension(const std::string& name) const final;
    virtual void Bind(const Render::Pass &pass);
    std::shared_ptr<Shader::Program> GetShader(const Render::Pass& pass);
    void SetShader(const Render::Pass& pass, std::shared_ptr<Shader::Program> shader);
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
    void bind_values(const Render::Pass& pass);
    void bind_textures(const Render::Pass& pass);
    std::array<std::shared_ptr<Shader::Program>, (int)Render::Pass::MaxValue> _shaders;
};