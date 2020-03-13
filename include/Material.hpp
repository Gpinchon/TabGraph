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
    static std::shared_ptr<Material> Get(unsigned index);
    static std::shared_ptr<Material> GetByName(const std::string&);
    static std::shared_ptr<Material> GetById(int64_t id);
    virtual void bind_values(std::shared_ptr<Shader> = nullptr);
    virtual void bind_textures(std::shared_ptr<Shader> = nullptr);
    std::shared_ptr<Shader> shader();
    std::shared_ptr<Shader> depth_shader();
    std::shared_ptr<Texture2D> texture_albedo();
    std::shared_ptr<Texture2D> texture_specular();
    std::shared_ptr<Texture2D> texture_emitting();
    std::shared_ptr<Texture2D> texture_normal();
    std::shared_ptr<Texture2D> texture_height();
    std::shared_ptr<Texture2D> texture_metallicRoughness();
    std::shared_ptr<Texture2D> texture_roughness();
    std::shared_ptr<Texture2D> texture_metallic();
    std::shared_ptr<Texture2D> texture_ao();
    void set_texture_albedo(std::shared_ptr<Texture2D>);
    void set_texture_specular(std::shared_ptr<Texture2D>);
    void set_texture_emitting(std::shared_ptr<Texture2D>);
    void set_texture_normal(std::shared_ptr<Texture2D>);
    void set_texture_height(std::shared_ptr<Texture2D>);
    void set_texture_metallicRoughness(std::shared_ptr<Texture2D>);
    void set_texture_roughness(std::shared_ptr<Texture2D>);
    void set_texture_metallic(std::shared_ptr<Texture2D>);
    void set_texture_ao(std::shared_ptr<Texture2D>);
    glm::vec3 albedo { 0, 0, 0 };
    glm::vec3 specular { 0.04, 0.04, 0.04 };
    glm::vec3 emitting { 0, 0, 0 };
    glm::vec2 uv_scale { 1, 1 };
    float roughness { 0.5 };
    float metallic { 0 };
    float alpha { 1 };
    float parallax { 0.01 };
    float ior { 1 };

protected:
    static std::vector<std::shared_ptr<Material>> _materials;
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
};