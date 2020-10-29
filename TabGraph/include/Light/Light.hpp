/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-12 12:05:01
*/

#pragma once

#include "Node.hpp" // for Node
#include "glm/glm.hpp" // for glm::vec3
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Framebuffer;

enum LightType {
    Point,
    Directionnal
};

class Light : public Node {
public:
    Light(const std::string& name);
    static std::shared_ptr<Light> Create(const std::string& name, glm::vec3 color, glm::vec3 position, float power);
    static std::shared_ptr<Light> GetByName(const std::string&);
    static std::shared_ptr<Light> Get(unsigned index);
    virtual void render_shadow();
    glm::vec3& color();
    float& power();
    bool& cast_shadow();
    std::shared_ptr<Framebuffer> render_buffer();
    virtual LightType type();
    virtual glm::mat4 ShadowProjectionMatrix() const;

protected:
    glm::vec3 _color { 0, 0, 0 };
    float _power { 0 };
    bool _cast_shadow { false };
    std::weak_ptr<Framebuffer> _render_buffer;
};

class DirectionnalLight : public Light {
public:
    DirectionnalLight(const std::string& name);
    static std::shared_ptr<DirectionnalLight> Create(const std::string& name, glm::vec3 color, glm::vec3 position, float power, bool cast_shadow = false);
    virtual void render_shadow() override;
    virtual LightType type() override;
    virtual glm::mat4 ShadowProjectionMatrix() const override;
    virtual void SetLimits(glm::vec4 limits);
    virtual glm::vec4 Limits() const;

protected:
    glm::vec4 _limits { -100, 100, -100, 100 };
};
