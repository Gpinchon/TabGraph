/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 18:02:54
*/

#pragma once

#include <memory>      // for shared_ptr, weak_ptr
#include <string>      // for string
#include <vector>      // for vector
#include "Object.hpp"  // for Object
#include "glm/glm.hpp"       // for glm::vec3, glm::vec2

class Shader;
class Texture;

class Material : public Object {
public:
    static std::shared_ptr<Material> create(const std::string&);
    static std::shared_ptr<Material> Get(unsigned index);
    static std::shared_ptr<Material> get_by_name(const std::string&);
    virtual void bind_values();
    virtual void bind_textures();
    std::shared_ptr<Shader> shader();
    std::shared_ptr<Shader> depth_shader();
    std::shared_ptr<Texture> texture_albedo();
    std::shared_ptr<Texture> texture_specular();
    std::shared_ptr<Texture> texture_emitting();
    std::shared_ptr<Texture> texture_normal();
    std::shared_ptr<Texture> texture_height();
    std::shared_ptr<Texture> texture_roughness();
    std::shared_ptr<Texture> texture_metallic();
    std::shared_ptr<Texture> texture_ao();
    void set_texture_albedo(std::shared_ptr<Texture>);
    void set_texture_specular(std::shared_ptr<Texture>);
    void set_texture_emitting(std::shared_ptr<Texture>);
    void set_texture_normal(std::shared_ptr<Texture>);
    void set_texture_height(std::shared_ptr<Texture>);
    void set_texture_roughness(std::shared_ptr<Texture>);
    void set_texture_metallic(std::shared_ptr<Texture>);
    void set_texture_ao(std::shared_ptr<Texture>);
    glm::vec3 albedo{ 0, 0, 0 };
    glm::vec3 specular{ 0.04, 0.04, 0.04 };
    glm::vec3 emitting{ 0, 0, 0 };
    glm::vec2 uv_scale{ 1, 1 };
    float roughness{ 0.5 };
    float metallic{ 0 };
    float alpha{ 1 };
    float parallax{ 0.01 };
    float ior{ 1 };

protected:
    static std::vector<std::shared_ptr<Material>> _materials;
    std::weak_ptr<Shader> _shader;
    std::weak_ptr<Shader> _depth_shader;
    std::weak_ptr<Texture> _texture_albedo;
    std::weak_ptr<Texture> _texture_specular;
    std::weak_ptr<Texture> _texture_emitting;
    std::weak_ptr<Texture> _texture_normal;
    std::weak_ptr<Texture> _texture_height;
    std::weak_ptr<Texture> _texture_roughness;
    std::weak_ptr<Texture> _texture_metallic;
    std::weak_ptr<Texture> _texture_ao;
    Material(const std::string& name);
};