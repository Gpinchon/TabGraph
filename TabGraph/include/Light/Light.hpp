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

class Light : public Node {
public:
    Light(const std::string& name, glm::vec3 color, glm::vec3 position, float power);
    Light();

    void SetColor(const glm::vec3& color);
    glm::vec3 Color() const;
    void SetPower(const float& color);
    float Power() const;
    void SetCastShadow(bool castShadow);
    bool CastShadow() const;
    virtual void render_shadow() = 0;
    virtual void Draw() = 0;

protected:
    glm::vec3 _color { 1 };
    float _power { 1 };
    bool _cast_shadow { false };
};
