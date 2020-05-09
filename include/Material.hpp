/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:09
*/

#pragma once

#include "Object.hpp" // for Object
#include "glm/glm.hpp" // for glm::vec3, glm::vec2
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Shader;
class Texture2D;

class Material : public Object {
public:
    static std::shared_ptr<Material> Create(const std::string&);
    virtual void Bind();
    std::shared_ptr<Shader> shader();
    std::shared_ptr<Shader> depth_shader();
    std::shared_ptr<Texture2D> TextureAlbedo();
    std::shared_ptr<Texture2D> TextureSpecular();
    std::shared_ptr<Texture2D> TextureEmitting();
    std::shared_ptr<Texture2D> TextureNormal();
    std::shared_ptr<Texture2D> TextureHeight();
    std::shared_ptr<Texture2D> TextureMetallicRoughness();
    std::shared_ptr<Texture2D> TextureRoughness();
    std::shared_ptr<Texture2D> TextureMetallic();
    std::shared_ptr<Texture2D> TextureAO();
    void SetTextureAlbedo(std::shared_ptr<Texture2D>);
    void SetTextureSpecular(std::shared_ptr<Texture2D>);
    void SetTextureEmitting(std::shared_ptr<Texture2D>);
    void SetTextureNormal(std::shared_ptr<Texture2D>);
    void SetTextureHeight(std::shared_ptr<Texture2D>);
    void SetTextureMetallicRoughness(std::shared_ptr<Texture2D>);
    void SetTextureRoughness(std::shared_ptr<Texture2D>);
    void SetTextureMetallic(std::shared_ptr<Texture2D>);
    void SetTextureAO(std::shared_ptr<Texture2D>);
    glm::vec3 Albedo() const;
    void SetAlbedo(glm::vec3);
    glm::vec3 Specular() const;
    void SetSpecular(glm::vec3);
    glm::vec3 Emitting() const;
    void SetEmitting(glm::vec3);
    glm::vec2 UVScale() const;
    void SetUVScale(glm::vec2);
    float Roughness() const;
    void SetRoughness(float);
    float Metallic() const;
    void SetMetallic(float);
    float Alpha() const;
    void SetAlpha(float);
    float Parallax() const;
    void SetParallax(float);
    float Ior() const;
    void SetIor(float);
    bool DoubleSided() const;
    void SetDoubleSided(bool doubleSided);

protected:
    std::weak_ptr<Shader> _shader;
    std::weak_ptr<Shader> _depth_shader;
    std::shared_ptr<Texture2D> _texture_albedo;
    std::shared_ptr<Texture2D> _texture_specular;
    std::shared_ptr<Texture2D> _texture_emitting;
    std::shared_ptr<Texture2D> _texture_normal;
    std::shared_ptr<Texture2D> _texture_height;
    std::shared_ptr<Texture2D> _texture_metallicRoughness;
    std::shared_ptr<Texture2D> _texture_roughness;
    std::shared_ptr<Texture2D> _texture_metallic;
    std::shared_ptr<Texture2D> _texture_ao;
    Material(const std::string& name);
    void bind_values();
    void bind_textures();
    glm::vec3 _albedo { 0, 0, 0 };
    glm::vec3 _specular { 0.04, 0.04, 0.04 };
    glm::vec3 _emitting { 0, 0, 0 };
    glm::vec2 _uv_scale { 1, 1 };
    float _roughness { 0.5 };
    float _metallic { 0 };
    float _alpha { 1 };
    float _parallax { 0.01 };
    float _ior { 1 };
    bool _doubleSided { false };
};