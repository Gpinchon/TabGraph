/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-01 20:19:21
*/

#pragma once

#include "Component.hpp"

#include <Property.hpp> // for PROPERTY, READONLYPROPERTY
#include <glm/ext/matrix_float4x4.hpp> // for mat4
#include <map>
#include <memory> // for shared_ptr, weak_ptr
#include <stdint.h> // for int64_t, uint32_t
#include <string> // for string

class Geometry;
class Material;
class BufferAccessor;
namespace Renderer {
class MeshRenderer;
}

class Mesh : public Component {
public:
    READONLYPROPERTY(bool, Loaded, false);
    PROPERTY(glm::mat4, GeometryTransform, 1);

public:
    Renderer::MeshRenderer& GetRenderer() const;

    Mesh();
    Mesh(const std::string& name);
    Mesh(const Mesh& other);
    /** Adds the Geometry to Geometrys list */
    void AddGeometry(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material);
    std::shared_ptr<Material> GetGeometryMaterial(std::shared_ptr<Geometry> geometry) const;
    void SetGeometryMaterial(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material);

    std::shared_ptr<BufferAccessor> GetWeights() const;
    void SetWeights(std::shared_ptr<BufferAccessor> weights);

    const std::map<std::shared_ptr<Geometry>, std::shared_ptr<Material>> GetGeometries() const;

    virtual void Load();

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<Mesh>(*this);
    }
    std::unique_ptr<Renderer::MeshRenderer> _renderer;
    std::map<std::shared_ptr<Geometry>, std::shared_ptr<Material>> _geometries;
};
