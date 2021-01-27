/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:25
*/

#pragma once

#include <glm/glm.hpp> // for glm::vec3
#include <memory> // for shared_ptr
#include <string> // for string

class Mesh;
class Geometry;

namespace CubeMesh {
std::shared_ptr<Mesh> Create(const std::string& name, glm::vec3 size);
std::shared_ptr<Geometry> CreateGeometry(const std::string& name, glm::vec3 size);
};