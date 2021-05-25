/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 16:31:00
*/

#include "Surface/Mesh.hpp"
#include "Material/Material.hpp"
#include "Surface/Geometry.hpp"

//#if RENDERINGAPI == OpenGL
#include "Driver/OpenGL/Renderer/Surface/MeshRenderer.hpp"
//#endif

#include <memory> // for shared_ptr
#include <stddef.h> // for size_t

size_t meshNbr(0);

Mesh::Mesh()
    : Mesh("Mesh_" + std::to_string(meshNbr))
{
    meshNbr++;
}

Mesh::Mesh(const std::string& name)
    : Surface(name)
{
    _renderer.reset(new Renderer::MeshRenderer(*this));
    meshNbr++;
}

Mesh::Mesh(const Mesh& other)
    : Surface(other)
    , _geometries(other._geometries)
{
    _renderer.reset(new Renderer::MeshRenderer(*this));
    SetGeometryTransform(other.GetGeometryTransform());
    meshNbr++;
}

Mesh::~Mesh()
{
}

void Mesh::AddGeometry(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material)
{
    return SetGeometryMaterial(geometry, material);
}

std::shared_ptr<Material> Mesh::GetGeometryMaterial(uint32_t geometryIndex) const
{
    auto itr { _geometries.find(geometryIndex) };
    return itr == _geometries.end() ? nullptr : GetComponent<Material>(itr->second);
}

std::shared_ptr<Material> Mesh::GetGeometryMaterial(std::shared_ptr<Geometry> geometry) const
{
    auto geometryIndex { GetComponentIndex<Geometry>(geometry) };
    auto itr { _geometries.find(geometryIndex) };
    return itr == _geometries.end() ? nullptr : GetComponent<Material>(itr->second);
}

void Mesh::SetGeometryMaterial(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material)
{
    _geometries[AddComponent(geometry)] = AddComponent(material);
}

void Mesh::Load()
{
    if (GetLoaded())
        return;
    _SetLoaded(true);
}

std::shared_ptr<BufferAccessor> Mesh::GetWeights() const
{
    return GetComponent<BufferAccessor>();
}

void Mesh::SetWeights(std::shared_ptr<BufferAccessor> weights)
{
    SetComponent(weights);
}

const std::vector<std::shared_ptr<Geometry>> Mesh::GetGeometries() const
{
    return GetComponents<Geometry>();
    //return _geometries;
}
