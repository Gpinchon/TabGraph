/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-14 19:22:07
*/

#pragma once

#include "Node.hpp" // for Node
#include "glm/glm.hpp" // for glm::vec3
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class Framebuffer;

class Light : public Node {
    PROPERTY(glm::vec3, Color, 1);
    PROPERTY(bool, CastShadow, false);

public:
    Light(const std::string& name, glm::vec3 color);
    Light();
    virtual void render_shadow() = 0;
    virtual void Draw() = 0;
};
