/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:26
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
    Material(const std::string& name);
    virtual void AddExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual void RemoveExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual std::shared_ptr<MaterialExtension> GetExtension(const std::string& name) const final;
    virtual void Bind();
    std::shared_ptr<Shader> GeometryShader();
    std::shared_ptr<Shader> MaterialShader();
    std::shared_ptr<Texture2D> TextureDiffuse() const;
    std::shared_ptr<Texture2D> TextureEmissive() const;
    std::shared_ptr<Texture2D> TextureNormal() const;
    std::shared_ptr<Texture2D> TextureHeight() const;
    std::shared_ptr<Texture2D> TextureAO() const;
    void SetTextureDiffuse(std::shared_ptr<Texture2D>);
    void SetTextureEmissive(std::shared_ptr<Texture2D>);
    void SetTextureNormal(std::shared_ptr<Texture2D>);
    void SetTextureHeight(std::shared_ptr<Texture2D>);
    void SetTextureAO(std::shared_ptr<Texture2D>);
    std::shared_ptr<Texture2D> BRDFLUT() const;
    void SetBRDFLUT(const std::shared_ptr<Texture2D>&);

protected:
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto clone = Component::Create<Material>(*this);
        clone->OpacityModeChanged.ConnectMember(clone.get(), &Material::_updateOpacityMode);
        clone->OpacityCutoffChanged.ConnectMember(clone.get(), &Material::_updateOpacityCutoff);
        clone->DiffuseChanged.ConnectMember(clone.get(), &Material::_updateDiffuse);
        clone->EmissiveChanged.ConnectMember(clone.get(), &Material::_updateEmissive);
        clone->UVScaleChanged.ConnectMember(clone.get(), &Material::_updateUVScale);
        clone->OpacityChanged.ConnectMember(clone.get(), &Material::_updateOpacity);
        clone->ParallaxChanged.ConnectMember(clone.get(), &Material::_updateParallax);
        clone->IorChanged.ConnectMember(clone.get(), &Material::_updateIor);
        return clone;
    }
    int64_t _geometryShader;
    int64_t _materialShader;
    int64_t _texture_diffuse;
    int64_t _texture_emissive;
    int64_t _texture_normal;
    int64_t _texture_height;
    int64_t _texture_ao;
    int64_t _brdfLUT;
    void bind_values();
    void bind_textures();
    void _updateOpacityMode(OpacityModeValue);
    void _updateOpacityCutoff(float);
    void _updateDiffuse(glm::vec3);
    void _updateEmissive(glm::vec3);
    void _updateUVScale(glm::vec2);
    void _updateOpacity(float);
    void _updateParallax(float);
    void _updateIor(float);
    PROPERTY(OpacityModeValue, OpacityMode, OpacityModeValue::Opaque);
    PROPERTY(float, OpacityCutoff, 0.5);
    PROPERTY(glm::vec3, Diffuse, 1);
    PROPERTY(glm::vec3, Emissive, 0);
    PROPERTY(glm::vec2, UVScale, 1);
    PROPERTY(float, Opacity, 1);
    PROPERTY(float, Parallax, 0.01);
    PROPERTY(float, Ior, 1);
    PROPERTY(bool, DoubleSided, false);
    PRIVATEPROPERTY(bool, ShaderChanged, false);
};