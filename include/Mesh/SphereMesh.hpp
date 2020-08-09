#pragma once

#include <glm/vec2.hpp>
#include <memory>

class Mesh;
class Geometry;

namespace SphereMesh {
std::shared_ptr<Mesh> Create(const std::string& name, float radius, unsigned subdivision = 2);
std::shared_ptr<Geometry> CreateGeometry(const std::string& name, float radius, unsigned subdivision = 2);
};