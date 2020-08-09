#pragma once

#include <memory>

class Mesh;
class Geometry;

namespace CapsuleMesh {
std::shared_ptr<Mesh> Create(const std::string& name, float heigth, float radius, int sectorCount = 32, int heightSubdivision = 10);
std::shared_ptr<Geometry> CreateGeometry(const std::string& name, float heigth, float radius, int sectorCount = 32, int heightSubdivision = 10);
}