/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 15:45:42
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
    static std::shared_ptr<Material> Create(const std::string&);
    virtual void AddExtension(std::shared_ptr<MaterialExtension> extension) final;
    virtual std::shared_ptr<MaterialExtension> GetExtension(const std::string& name) const final;
    virtual void Bind();
    void SetShader(std::shared_ptr<Shader>);
    std::shared_ptr<Shader> shader();
    std::shared_ptr<Shader> depth_shader();
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
    glm::vec3 Diffuse() const;
    void SetDiffuse(glm::vec3);
    glm::vec3 Emissive() const;
    void SetEmissive(glm::vec3);
    glm::vec2 UVScale() const;
    void SetUVScale(glm::vec2);
    float Opacity() const;
    void SetOpacity(float);
    float Parallax() const;
    void SetParallax(float);
    float Ior() const;
    void SetIor(float);
    bool DoubleSided() const;
    void SetDoubleSided(bool doubleSided);

protected:
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateGPU(float /*delta*/) override;
    std::shared_ptr<Shader> _shader;
    std::shared_ptr<Shader> _depth_shader;
    std::shared_ptr<Texture2D> _texture_diffuse;
    std::shared_ptr<Texture2D> _texture_emissive;
    std::shared_ptr<Texture2D> _texture_normal;
    std::shared_ptr<Texture2D> _texture_height;
    std::shared_ptr<Texture2D> _texture_ao;
    Material(const std::string& name);
    void bind_values();
    void bind_textures();
    glm::vec3 _diffuse{ 1, 1, 1 };
    glm::vec3 _emissive { 0, 0, 0 };
    glm::vec2 _uv_scale { 1, 1 };
    float _opacity { 1 };
    float _parallax { 0.01 };
    float _ior { 1 };
    bool _doubleSided { false };
    bool _shaderChanged{ false };
};