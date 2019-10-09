/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-07 14:28:53
*/

#include "Mesh.hpp"
#include "AABB.hpp" // for AABB
#include "BoundingElement.hpp" // for BoundingElement
#include "Camera.hpp" // for Camera
#include "Debug.hpp"
#include "Material.hpp" // for Material
#include "Node.hpp" // for Node
#include "Shader.hpp" // for Shader
#include "Texture.hpp"
#include "Vgroup.hpp" // for Vgroup
#include <glm/gtc/matrix_inverse.hpp>

std::vector<std::shared_ptr<Mesh>> Mesh::_meshes;

Mesh::Mesh(const std::string &name)
    : Renderable(name)
{
    bounding_element = new AABB;
}

std::shared_ptr<Mesh> Mesh::Create(const std::string &name) /*static*/
{
    auto m = std::shared_ptr<Mesh>(new Mesh(name));
    Mesh::Add(m);
    return (m);
}

std::shared_ptr<Mesh> Mesh::GetByName(const std::string &name) /*static*/
{
    for (auto m : _meshes)
    {
        if (name == m->Name())
            return (m);
    }
    return (nullptr);
}

std::shared_ptr<Mesh> Mesh::GetById(int64_t id)
{
    for (auto m : _meshes)
    {
        if (id == m->Id())
            return (m);
    }
    return (nullptr);
}

std::shared_ptr<Mesh> Mesh::Get(unsigned index) /*static*/
{
    if (index >= _meshes.size())
        return (nullptr);
    return (_meshes.at(index));
}

void Mesh::Add(std::shared_ptr<Mesh> mesh) /*static*/
{
    Renderable::Add(mesh);
    _meshes.push_back(mesh);
}

const std::set<std::shared_ptr<Vgroup>> Mesh::vgroups()
{
    return (_vgroups);
}

void Mesh::AddVgroup(std::shared_ptr<Vgroup> group)
{
    if (nullptr == group)
        return;
    _vgroups.insert(group);
}

void Mesh::Load()
{
    if (_is_loaded)
        return;
    for (auto vg : _vgroups)
    {
        if (nullptr == vg)
            continue;
        vg->Load();
    }
}

bool Mesh::DrawDepth(RenderMod mod)
{
    auto geometryTranslationMatrix(glm::translate(glm::mat4(1.f), GeometryPosition()));
    auto geometryRotationMatrix(glm::mat4(1.f));
    geometryRotationMatrix = glm::rotate(geometryRotationMatrix, glm::radians(GeometryRotation().y), glm::vec3(0, 1, 0));
    geometryRotationMatrix = glm::rotate(geometryRotationMatrix, glm::radians(GeometryRotation().z), glm::vec3(0, 0, 1));
    geometryRotationMatrix = glm::rotate(geometryRotationMatrix, glm::radians(GeometryRotation().x), glm::vec3(1, 0, 0));
    auto geometryScalingMatrix(glm::scale(glm::mat4(1.f), Scale()));
    auto geometryTransformMatrix(geometryTranslationMatrix * geometryRotationMatrix * geometryScalingMatrix);

    auto finalTranformMatrix(geometryTransformMatrix * TransformMatrix());

    bool ret = false;
    auto mvp = Camera::current()->ProjectionMatrix() * Camera::current()->ViewMatrix() * finalTranformMatrix;
    auto normal_matrix = glm::inverseTranspose(finalTranformMatrix);

    Load();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : _vgroups)
    {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material)
            continue;
        if (mod == RenderOpaque
            && (material->alpha < 1 || (material->texture_albedo() != nullptr && material->texture_albedo()->values_per_pixel() == 4)))
            continue;
        else if (mod == RenderTransparent
                 && !(material->alpha < 1 || (material->texture_albedo() != nullptr && material->texture_albedo()->values_per_pixel() == 4)))
            continue;
        auto shader(material->depth_shader());
        if (nullptr == shader)
            continue;
        shader->use();
        if (last_shader != shader)
        {
            shader->set_uniform("Matrix.Model", finalTranformMatrix);
            shader->set_uniform("Matrix.ModelViewProjection", mvp);
            shader->set_uniform("Matrix.Normal", normal_matrix);
            last_shader = shader;
        }
        material->bind_textures();
        material->bind_values();
        if (vg->Draw())
            ret = true;
        shader->use(false);
    }
    return (ret);
}

bool Mesh::Draw(RenderMod mod)
{
    auto finalTranformMatrix(TransformMatrix());
    finalTranformMatrix = glm::translate(finalTranformMatrix, GeometryPosition());
    finalTranformMatrix = glm::rotate(finalTranformMatrix, glm::radians(GeometryRotation().y), glm::vec3(0, 1, 0));
    finalTranformMatrix = glm::rotate(finalTranformMatrix, glm::radians(GeometryRotation().z), glm::vec3(0, 0, 1));
    finalTranformMatrix = glm::rotate(finalTranformMatrix, glm::radians(GeometryRotation().x), glm::vec3(1, 0, 0));
    finalTranformMatrix = glm::scale(finalTranformMatrix, Scale());

    bool ret = false;
    auto mvp = Camera::current()->ProjectionMatrix() * Camera::current()->ViewMatrix() * finalTranformMatrix;
    auto normal_matrix = glm::inverseTranspose(finalTranformMatrix);

    //auto geometryTransform()

    Load();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : _vgroups)
    {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material)
        {
            errorLog("Error : Invalid Material Index while rendering Mesh" + Name());
            continue;
        }
        if (mod == RenderOpaque
            && (material->alpha < 1 || (material->texture_albedo() != nullptr && material->texture_albedo()->values_per_pixel() == 4)))
            continue;
        else if (mod == RenderTransparent
                 && !(material->alpha < 1 || (material->texture_albedo() != nullptr && material->texture_albedo()->values_per_pixel() == 4)))
            continue;
        auto shader(material->shader());
        if (nullptr == shader)
            continue;
        shader->use();
        if (last_shader != shader)
        {
            shader->set_uniform("Matrix.Model", finalTranformMatrix);
            shader->set_uniform("Matrix.ModelViewProjection", mvp);
            shader->set_uniform("Matrix.Normal", normal_matrix);
            last_shader = shader;
        }
        material->bind_textures();
        material->bind_values();
        if (vg->Draw())
            ret = true;
        shader->use(false);
    }
    return (ret);
}

bool Mesh::Drawable() const
{
    return true;
}

void Mesh::set_cull_mod(GLenum mod)
{
    _cull_mod = mod;
}

/*void	Mesh::sort(renderable_compare compare)
{
	std::sort(_vgroups.begin(), _vgroups.end(), compare);
}*/

void Mesh::center()
{
    //TODO : FIX THE CENTER FUNCTION
    /*for (auto vg : _vgroups)
    {
        auto vg = vg.lock();
        if (nullptr == vg)
            continue;
        vg->center(bounding_element->center);
    }
    bounding_element->min = bounding_element->min - bounding_element->center;
    bounding_element->max = bounding_element->max - bounding_element->center;
    bounding_element->center = glm::vec3(0, 0, 0);*/
}

void Mesh::AddMaterial(std::shared_ptr<Material> material)
{
    _materials.push_back(material);
}

void Mesh::RemoveMaterial(std::shared_ptr<Material>)
{
}

void Mesh::SetMaterial(std::shared_ptr<Material> material, uint32_t index)
{
    if (index >= _materials.size())
        _materials.resize(index + 1);
    _materials.at(index) = material;
}

std::shared_ptr<Material> Mesh::GetMaterial(uint32_t index)
{
    return index >= _materials.size() ? nullptr : _materials.at(index);
}

int64_t Mesh::GetMaterialIndex(std::shared_ptr<Material> mtl)
{
    for (auto i(0u); i < _materials.size(); i++)
    {
        if (_materials.at(i) == mtl)
            return i;
    }
    return -1;
}

int64_t Mesh::GetMaterialIndex(const std::string &name)
{
    return GetMaterialIndex(Material::GetByName(name));
}


glm::vec3 Mesh::GeometryPosition() const 
{
    return _geometryPosition;
}

void Mesh::SetGeometryPosition(glm::vec3 position)
{
    _geometryPosition = position;
}

glm::vec3 Mesh::GeometryRotation() const 
{
    return _geometryRotation;
}

void Mesh::SetGeometryRotation(glm::vec3 rotation)
{
    _geometryRotation = rotation;
}

glm::vec3 Mesh::GeometryScale() const 
{
    return _geometryScale;
}

void Mesh::SetGeometryScale(glm::vec3 scale)
{
    _geometryScale = scale;
}