/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:08
*/

#pragma once

#include "glm/glm.hpp" // for glm::vec3
#include <memory> // for shared_ptr
#include <string> // for string

class Mesh;

namespace PlaneMesh {
std::shared_ptr<Mesh> Create(const std::string& name, glm::vec2 size, unsigned subdivision = 1);
};