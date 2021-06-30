/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 14:14:44
*/

#pragma once

#include <Core/Inherit.hpp>
#include <Shapes/Shape.hpp>

#include <glm/ext/matrix_float4x4.hpp> // for mat4
#include <map>
#include <memory> // for shared_ptr, weak_ptr
#include <stdint.h> // for int64_t, uint32_t
#include <string> // for string
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Geometry;
}
namespace TabGraph::Memory {
class BufferAccessor;
}
namespace TabGraph::Materials {
class Material;
}

////////////////////////////////////////////////////////////////////////////////
//Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Mesh : public TabGraph::Core::Inherit<Shape, Mesh> {
public:
    Mesh();
    Mesh(const std::string& name);
    Mesh(const Mesh& other);
    ~Mesh();

    /** Adds the Geometry to Geometrys list */
    void AddGeometry(std::shared_ptr<Geometry> geometry, std::shared_ptr<Materials::Material> material);
    std::shared_ptr<Materials::Material> GetGeometryMaterial(uint32_t geometryIndex) const;
    std::shared_ptr<Materials::Material> GetGeometryMaterial(std::shared_ptr<Geometry> geometry) const;
    void SetGeometryMaterial(std::shared_ptr<Geometry> geometry, std::shared_ptr<Materials::Material> material);

    std::shared_ptr<Memory::BufferAccessor> GetWeights() const {
        return _weights;
    }

    void SetWeights(std::shared_ptr<Memory::BufferAccessor> weights) {
        _weights = weights;
    }

    const auto& GetGeometries() const {
        return _geometries;
    }

    virtual void Load();
    inline bool GetLoaded() {
        return _loaded;
    }
    inline auto& GetGeometryTransform() const {
        return _geometryTransform;
    }
    inline void SetGeometryTransform(const glm::mat4& transform) {
        _geometryTransform = transform;
    }

private:
    std::map<std::shared_ptr<Geometry>, std::shared_ptr<Materials::Material>> _geometries;
    std::shared_ptr<Memory::BufferAccessor> _weights;
    bool _loaded{ false };
    glm::mat4 _geometryTransform{ 1 };
};
}
