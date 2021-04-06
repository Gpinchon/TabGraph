/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-01 20:19:31
*/

#include "Mesh/Mesh.hpp"
#include "Material/Material.hpp"
#include "Mesh/Geometry.hpp"
#include "Renderer/MeshRenderer.hpp"

#include <memory> // for shared_ptr
#include <stddef.h> // for size_t

size_t meshNbr(0);

Mesh::Mesh()
    : Mesh("Mesh_" + std::to_string(meshNbr))
{
    meshNbr++;
}

Mesh::Mesh(const std::string& name)
    : Component(name)
    , _renderer(new Renderer::MeshRenderer(*this))
{
    meshNbr++;
}

Mesh::Mesh(const Mesh& other)
    : Component(other)
    , _renderer(new Renderer::MeshRenderer(*this))
{
    SetGeometryTransform(other.GetGeometryTransform());
    meshNbr++;
}

Renderer::MeshRenderer& Mesh::GetRenderer() const
{
    return *_renderer;
}

void Mesh::AddGeometry(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material)
{
    return SetGeometryMaterial(geometry, material);
}

std::shared_ptr<Material> Mesh::GetGeometryMaterial(std::shared_ptr<Geometry> geometry) const
{
    auto itr{ _geometries.find(geometry) };
    return itr == _geometries.end() ? nullptr : itr->second;
}

void Mesh::SetGeometryMaterial(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material)
{
    _geometries[geometry] = material;
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

const std::map<std::shared_ptr<Geometry>, std::shared_ptr<Material>> Mesh::GetGeometries() const
{
    return _geometries;
}
