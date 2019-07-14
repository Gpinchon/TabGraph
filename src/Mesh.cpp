/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-07-14 22:47:46
*/

#include "Mesh.hpp"
#include "AABB.hpp" // for AABB
#include "BoundingElement.hpp" // for BoundingElement
#include "Camera.hpp" // for Camera
#include "Material.hpp" // for Material
#include "Node.hpp" // for Node
#include "Shader.hpp" // for Shader
#include "Vgroup.hpp" // for Vgroup
#include <glm/gtc/matrix_inverse.hpp>

std::vector<std::shared_ptr<Mesh>> Mesh::_meshes;

Mesh::Mesh(const std::string& name)
    : Renderable(name)
{
    bounding_element = new AABB;
}

std::shared_ptr<Mesh> Mesh::create(const std::string& name) /*static*/
{
    auto m = std::shared_ptr<Mesh>(new Mesh(name));
    Mesh::add(m);
    Renderable::add(m);
    Node::add(m);
    return (m);
}

std::shared_ptr<Mesh> Mesh::get_by_name(const std::string& name) /*static*/
{
    for (auto n : _meshes) {
        if (name == n->name())
            return (n);
    }
    return (nullptr);
}

std::shared_ptr<Mesh> Mesh::Get(unsigned index) /*static*/
{
    if (index >= _meshes.size())
        return (nullptr);
    return (_meshes.at(index));
}

void Mesh::add(std::shared_ptr<Mesh> mesh) /*static*/
{
    _meshes.push_back(mesh);
}

std::shared_ptr<Vgroup> Mesh::vgroup(unsigned index)
{
    if (index >= _vgroups.size())
        return (nullptr);
    return (_vgroups.at(index).lock());
}

void Mesh::add(std::shared_ptr<Vgroup> group)
{
    if (nullptr == group)
        return;
    _vgroups.push_back(group);
}

void Mesh::load()
{
    if (_is_loaded)
        return;
    for (auto vg : _vgroups) {
        auto vgPtr = vg.lock();
        if (nullptr == vgPtr)
            continue;
        vgPtr->load();
    }
}

bool Mesh::render_depth(RenderMod mod)
{
    bool ret = false;
    auto mvp = Camera::current()->projection() * Camera::current()->view() * TransformMatrix();
    auto normal_matrix = glm::inverseTranspose(TransformMatrix());

    load();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : _vgroups) {
        auto vgPtr = vg.lock();
        if (nullptr == vgPtr)
            continue;
        auto vgMaterial = vgPtr->material();
        if (vgMaterial == nullptr)
            continue;
        auto depthShader = vgMaterial->depth_shader();
        depthShader->use();
        if (last_shader != depthShader) {
            depthShader->set_uniform("Matrix.Model", TransformMatrix());
            depthShader->set_uniform("Matrix.ModelViewProjection", mvp);
            depthShader->set_uniform("Matrix.Normal", normal_matrix);
        }
        //depthShader->use(false);
        if (vgPtr->render_depth(mod))
            ret = true;
        last_shader = depthShader;
    }
    return (ret);
}

bool Mesh::render(RenderMod mod)
{
    bool ret = false;
    auto mvp = Camera::current()->projection() * Camera::current()->view() * TransformMatrix();
    auto normal_matrix = glm::inverseTranspose(TransformMatrix());

    load();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : _vgroups) {
        auto vgPtr = vg.lock();
        if (nullptr == vgPtr)
            continue;
        auto vgMaterial = vgPtr->material();
        if (vgMaterial == nullptr)
            continue;
        auto vgShader = vgMaterial->shader();
        vgShader->use();
        if (last_shader != vgShader) {
            vgShader->set_uniform("Matrix.Model", TransformMatrix());
            vgShader->set_uniform("Matrix.ModelViewProjection", mvp);
            vgShader->set_uniform("Matrix.Normal", normal_matrix);
        }
        if (vgPtr->render(mod))
            ret = true;
        last_shader = vgShader;
    }
    return (ret);
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
    for (auto vg : _vgroups) {
        auto vgPtr = vg.lock();
        if (nullptr == vgPtr)
            continue;
        vgPtr->center(bounding_element->center);
    }
    bounding_element->min = bounding_element->min - bounding_element->center;
    bounding_element->max = bounding_element->max - bounding_element->center;
    bounding_element->center = glm::vec3(0, 0, 0);
}
