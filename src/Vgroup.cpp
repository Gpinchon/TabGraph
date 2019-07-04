/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-28 13:44:30
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

Vgroup::Vgroup(const std::string& name)
    : Renderable(name)
{
    bounding_element = new AABB;
}

std::shared_ptr<Vgroup> Vgroup::create(const std::string& name)
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

std::shared_ptr<Vgroup> Vgroup::get_by_name(const std::string& name)
{
    for (auto n : _vgroups) {
        if (name == n->name())
            return (n);
    }
    return (nullptr);
}

void Vgroup::load()
{
    if (is_loaded()) {
        return;
    }
    _vao = VertexArray::create(v.size());
    auto vaoPtr = _vao.lock();
    vaoPtr->add_buffer(GL_FLOAT, 3, v);
    vaoPtr->add_buffer(GL_UNSIGNED_BYTE, 4, vn);
    vaoPtr->add_buffer(GL_FLOAT, 2, vt);
    vaoPtr->add_indices(i);
    _is_loaded = true;
}

void Vgroup::bind()
{
    if ((material() == nullptr) || (material()->shader() == nullptr) || (Camera::current() == nullptr)) {
        return;
    }
}

bool Vgroup::render_depth(RenderMod mod)
{
    auto mtl = material();
    if ((mtl == nullptr)) {
        return (false);
    }
    auto depthShader = mtl->depth_shader();
    if (depthShader == nullptr) {
        return (false);
    }
    if (mod == RenderOpaque
        && (mtl->alpha < 1 || (mtl->texture_albedo() != nullptr && mtl->texture_albedo()->values_per_pixel() == 4)))
        return (false);
    else if (mod == RenderTransparent
        && !(mtl->alpha < 1 || (mtl->texture_albedo() != nullptr && mtl->texture_albedo()->values_per_pixel() == 4))) {
        return (false);
    }
    auto vaoPtr = _vao.lock();
    if (nullptr == vaoPtr)
        return (false);
    depthShader->use();
    depthShader->bind_texture("Texture.Albedo", mtl->texture_albedo(), GL_TEXTURE0);
    depthShader->set_uniform("Texture.Use_Albedo", mtl->texture_albedo() != nullptr);
    depthShader->set_uniform("Material.Alpha", mtl->alpha);
    /*material->bind_textures();
	material->bind_values();*/
    //bind();
    vaoPtr->draw();
    depthShader->use(false);
    return (true);
}

bool Vgroup::render(RenderMod mod)
{
    auto mtl = material();
    if ((mtl == nullptr)) {
        return (false);
    }
    auto cShader = mtl->shader();
    if (cShader == nullptr) {
        return (false);
    }
    if (mod == RenderOpaque
        && (mtl->alpha < 1 || (mtl->texture_albedo() != nullptr && mtl->texture_albedo()->values_per_pixel() == 4)))
        return (false);
    else if (mod == RenderTransparent
        && !(mtl->alpha < 1 || (mtl->texture_albedo() != nullptr && mtl->texture_albedo()->values_per_pixel() == 4))) {
        return (false);
    }
    auto vaoPtr = _vao.lock();
    if (nullptr == vaoPtr)
        return (false);
    cShader->use();
    mtl->bind_textures();
    mtl->bind_values();
    bind();
    vaoPtr->draw();
    cShader->use(false);
    return (true);
}

void Vgroup::center(glm::vec3& center)
{
    for (auto& vec : v) {
        vec = vec - center;
    }
    bounding_element->min = bounding_element->min - center;
    bounding_element->max = bounding_element->max - center;
    bounding_element->center = bounding_element->center - center;
    position() = bounding_element->center - center;
}

void Vgroup::set_material(std::shared_ptr<Material> mtl)
{
    if (mtl != nullptr)
        _material = mtl;
    else
        _material.reset();
}

std::shared_ptr<Material> Vgroup::material()
{
    return (_material.lock());
}
