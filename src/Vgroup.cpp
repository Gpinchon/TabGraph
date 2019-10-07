/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-07 11:38:13
*/

#include "Vgroup.hpp"
#include "AABB.hpp" // for AABB
#include "BoundingElement.hpp" // for BoundingElement
#include "Camera.hpp" // for Camera
#include "Material.hpp" // for Material
#include "Shader.hpp" // for Shader
#include "Texture.hpp" // for Texture
#include "VertexArray.hpp" // for VertexArray

std::vector<std::shared_ptr<Vgroup>> Vgroup::_vgroups;

Vgroup::Vgroup(const std::string &name)
    : Object(name)
{
    boundingElement = new AABB;
}

std::shared_ptr<Vgroup> Vgroup::Create(const std::string &name)
{
    auto vg = std::shared_ptr<Vgroup>(new Vgroup(name));
    _vgroups.push_back(vg);
    return (vg);
}

std::shared_ptr<Vgroup> Vgroup::Get(unsigned index)
{
    if (index >= _vgroups.size())
        return (nullptr);
    return (_vgroups.at(index));
}

std::shared_ptr<Vgroup> Vgroup::GetByName(const std::string &name)
{
    for (auto n : _vgroups)
    {
        if (name == n->Name())
            return (n);
    }
    return (nullptr);
}

std::shared_ptr<Vgroup> Vgroup::GetById(int64_t id)
{
    for (auto n : _vgroups)
    {
        if (id == n->Id())
            return (n);
    }
    return (nullptr);
}

void Vgroup::Load()
{
    if (IsLoaded())
    {
        return;
    }
    _vao = VertexArray::Create(v.size());
    auto vaoPtr = _vao.lock();
    vaoPtr->add_buffer(GL_FLOAT, 3, v);
    vaoPtr->add_buffer(GL_UNSIGNED_BYTE, 4, vn);
    vaoPtr->add_buffer(GL_FLOAT, 2, vt);
    vaoPtr->add_indices(i);
    _isLoaded = true;
}

bool Vgroup::IsLoaded() const
{
    return _isLoaded;
}

bool Vgroup::Draw()
{
    auto vaoPtr = _vao.lock();
    if (nullptr == vaoPtr)
        return (false);
    vaoPtr->draw();
    return (true);
}

uint32_t Vgroup::MaterialIndex()
{
    return _materialIndex;
}
void Vgroup::SetMaterialIndex(uint32_t index)
{
    _materialIndex = index;
}

/*void Vgroup::center(glm::vec3 &center)
{
    for (auto &vec : v)
    {
        vec = vec - center;
    }
    boundingElement->min = boundingElement->min - center;
    boundingElement->max = boundingElement->max - center;
    boundingElement->center = boundingElement->center - center;
    SetPosition(boundingElement->center - center);
}*/

/*void Vgroup::set_material(std::shared_ptr<Material> mtl)
{
    if (mtl != nullptr)
        _material = mtl;
    else
        _material.reset();
}

std::shared_ptr<Material> Vgroup::material()
{
    return (_material.lock());
}*/
