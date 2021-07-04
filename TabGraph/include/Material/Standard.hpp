/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-03 20:05:02
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Property.hpp>
#include <Material/Extension.hpp>
#include <Renderer/Renderer.hpp>

#include <array>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Shader {
class Program;
}
namespace Textures {
class Texture2D;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Material {
class Standard : public Core::Inherit<Extension, Standard> {
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
    PROPERTY(glm::vec2, UVOffset, 0);
    PROPERTY(float, UVRotation, 0);

public:
    Standard(const std::string& name);
    virtual void AddExtension(std::shared_ptr<Extension> extension) final;
    virtual void RemoveExtension(std::shared_ptr<Extension> extension) final;
    virtual std::shared_ptr<Extension> GetExtension(const std::string& name) const final;
    virtual void Bind(const Renderer::Options::Pass& pass);
    std::shared_ptr<Shader::Program> GetShader(const Renderer::Options::Pass& pass);
    void SetShader(const Renderer::Options::Pass& pass, std::shared_ptr<Shader::Program> shader);

    glm::vec3 GetDiffuse(void);
    glm::vec3 GetEmissive(void);
    float GetOpacityCutoff(void);
    float GetOpacity(void);
    float GetParallax(void);
    float GetIor(void);
    std::shared_ptr<Textures::Texture2D> GetTextureDiffuse(void) const;
    std::shared_ptr<Textures::Texture2D> GetTextureEmissive(void) const;
    std::shared_ptr<Textures::Texture2D> GetTextureNormal(void) const;
    std::shared_ptr<Textures::Texture2D> GetTextureHeight(void) const;
    std::shared_ptr<Textures::Texture2D> GetTextureAO(void) const;
    std::shared_ptr<Textures::Texture2D> GetTextureBRDFLUT(void) const;

    void SetDiffuse(glm::vec3 value);
    void SetEmissive(glm::vec3 value);
    void SetOpacityCutoff(float value);
    void SetOpacity(float value);
    void SetParallax(float value);
    void SetIor(float value);
    void SetTextureDiffuse(std::shared_ptr<Textures::Texture2D>);
    void SetTextureEmissive(std::shared_ptr<Textures::Texture2D>);
    void SetTextureNormal(std::shared_ptr<Textures::Texture2D>);
    void SetTextureHeight(std::shared_ptr<Textures::Texture2D>);
    void SetTextureAO(std::shared_ptr<Textures::Texture2D>);
    void SetTextureBRDFLUT(std::shared_ptr<Textures::Texture2D>);

protected:
    PRIVATEPROPERTY(bool, ShaderChanged, false);
    std::array<std::shared_ptr<Shader::Program>, (int)Renderer::Options::Pass::MaxValue> _shaders;
    std::set<std::shared_ptr<Extension>> _extensions;
};
}
