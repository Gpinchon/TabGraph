/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   Gpinchon
* @Last Modified time: 2020-08-27 17:19:14
*/

#include "Mesh/Mesh.hpp"
#include "Buffer/BufferHelper.hpp"
#include "Camera/Camera.hpp" // for Camera
#include "Debug.hpp"
#include "Material.hpp" // for Material
#include "Mesh/Geometry.hpp" // for Geometry
#include "Mesh/MeshSkin.hpp"
#include "Node.hpp" // for Node
#include "Physics/BoundingAABB.hpp" // for BoundingAABB
#include "Physics/BoundingElement.hpp" // for BoundingElement
#include "Scene/Scene.hpp"
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Texture2D.hpp"
#include "Texture/TextureBuffer.hpp"
#include "Transform.hpp"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>

Mesh::Mesh(const std::string& name)
    : Component(name)
{
}

std::shared_ptr<Mesh> Mesh::Create(std::shared_ptr<Mesh> otherMesh) /*static*/
{
    std::shared_ptr<Mesh> newMesh(new Mesh(*otherMesh));
    //newMesh->SetTransform(Transform::Create(newMesh->Name() + "_geometryTransform"));
    return newMesh;
}

std::shared_ptr<Mesh> Mesh::Create(const std::string& name) /*static*/
{
    return std::shared_ptr<Mesh>(new Mesh(name));
}

std::shared_ptr<Mesh> Mesh::Create() /*static*/
{
    static uint64_t meshNbr(0);
    ++meshNbr;
    return std::shared_ptr<Mesh>(new Mesh("Mesh_" + std::to_string(meshNbr)));
}

const std::set<std::shared_ptr<Geometry>> Mesh::Geometrys()
{
    return (_Geometrys);
}

void Mesh::AddGeometry(std::shared_ptr<Geometry> group)
{
    if (nullptr == group)
        return;
    _Geometrys.insert(group);
}

void Mesh::_LoadGPU()
{
    if (LoadedGPU())
        return;
    debugLog(Name());
    for (auto vg : _Geometrys)
        vg->LoadGPU();
    if (_jointMatrices != nullptr)
        _jointMatrices->load();
    SetLoadedGPU(true);
}

bool Mesh::DrawDepth(const std::shared_ptr<Transform>& transform, RenderMod mod)
{
    auto currentCamera(Scene::Current() ? Scene::Current()->CurrentCamera() : nullptr);
    auto geometryTransform(GetComponent<Transform>());
    auto finalTranformMatrix(transform->WorldTransformMatrix() * (geometryTransform ? geometryTransform->WorldTransformMatrix() : glm::mat4(1.f)));

    bool ret = false;
    auto viewProjectionMatrix = currentCamera->ProjectionMatrix() * currentCamera->ViewMatrix();
    auto normal_matrix = glm::inverseTranspose(finalTranformMatrix);

    LoadGPU();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : _Geometrys) {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material)
            continue;
        auto isTransparent(material->Alpha() < 1); // || (material->TextureAlbedo() != nullptr && material->TextureAlbedo()->values_per_pixel() == 4));
        if (mod == RenderMod::RenderOpaque && isTransparent)
            continue;
        else if (mod == RenderMod::RenderTransparent && !isTransparent)
            continue;
        if (nullptr == material->depth_shader())
            continue;
        auto shader(material->depth_shader());
        material->Bind();
        if (last_shader != shader) {
            shader->SetUniform("Camera.Position", Scene::Current()->CurrentCamera()->GetComponent<Transform>()->WorldPosition());
            shader->SetUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
            shader->SetUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
            shader->SetUniform("Camera.Matrix.ViewProjection", viewProjectionMatrix);
            shader->SetUniform("Matrix.Model", finalTranformMatrix);
            shader->SetUniform("Matrix.Normal", normal_matrix);
            if (GetComponent<MeshSkin>() != nullptr) {
                shader->SetUniform("Joints", _jointMatrices, GL_TEXTURE11);
                shader->SetUniform("Skinned", true);
            } else {
                shader->SetUniform("Joints", nullptr, GL_TEXTURE11);
                shader->SetUniform("Skinned", false);
            }
            last_shader = shader;
        }
        shader->use();
        ret |= vg->Draw();
        shader->use(false);
    }
    return ret;
}

bool Mesh::Draw(const std::shared_ptr<Transform>& transform, RenderMod mod)
{
    auto currentCamera(Scene::Current() ? Scene::Current()->CurrentCamera() : nullptr);
    auto geometryTransform(GetComponent<Transform>());
    auto finalTranformMatrix(transform->WorldTransformMatrix() * (geometryTransform ? geometryTransform->WorldTransformMatrix() : glm::mat4(1.f)));

    bool ret = false;
    auto viewProjectionMatrix = currentCamera->ProjectionMatrix() * currentCamera->ViewMatrix();
    auto normal_matrix = glm::inverseTranspose(finalTranformMatrix);

    LoadGPU();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : _Geometrys) {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material) {
            errorLog("Error : Invalid Material Index while rendering Mesh");
            continue;
        }
        if (mod == RenderMod::RenderOpaque
            && (material->Alpha() < 1 || (material->TextureAlbedo() != nullptr && material->TextureAlbedo()->values_per_pixel() == 4)))
            continue;
        else if (mod == RenderMod::RenderTransparent
            && !(material->Alpha() < 1 || (material->TextureAlbedo() != nullptr && material->TextureAlbedo()->values_per_pixel() == 4)))
            continue;
        if (nullptr == material->shader())
            continue;
        auto shader(material->shader());
        material->Bind();
        if (last_shader != shader) {
            shader->SetUniform("Camera.Matrix.ViewProjection", viewProjectionMatrix);
            shader->SetUniform("Matrix.Model", finalTranformMatrix);
            shader->SetUniform("Matrix.Normal", normal_matrix);
            if (GetComponent<MeshSkin>() != nullptr) {
                shader->SetUniform("Joints", _jointMatrices, GL_TEXTURE11);
                shader->SetUniform("Skinned", true);
            } else {
                shader->SetUniform("Joints", nullptr, GL_TEXTURE11);
                shader->SetUniform("Skinned", false);
            }
            last_shader = shader;
        }
        if (material->DoubleSided())
            glDisable(GL_CULL_FACE);
        else
            glEnable(GL_CULL_FACE);
        shader->use();
        ret |= vg->Draw();
        shader->use(false);
        glEnable(GL_CULL_FACE);
    }
    return ret;
}

bool Mesh::Drawable() const
{
    return true;
}

void Mesh::set_cull_mod(GLenum mod)
{
    _cull_mod = mod;
}

void Mesh::center()
{
    //TODO : FIX THE CENTER FUNCTION
    /*for (auto vg : _Geometrys)
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
    for (auto i(0u); i < _materials.size(); ++i) {
        if (_materials.at(i) == mtl)
            return i;
    }
    return -1;
}

int64_t Mesh::GetMaterialIndex(const std::string& name)
{
    for (auto i(0u); i < _materials.size(); ++i) {
        if (_materials.at(i)->Name() == name)
            return i;
    }
    return -1;
}

void Mesh::_FixedUpdateGPU(float delta)
{
    if (_jointMatrices != nullptr) {
        _jointMatrices->Accessor()->GetBufferView()->GetBuffer()->UpdateGPU(delta);
    }
    SetNeedsFixedUpdateGPU(false);
}

void Mesh::UpdateSkin(const std::shared_ptr<Transform>& transform)
{
    if (GetComponent<MeshSkin>() == nullptr)
        return;
    bool skinChanged(false);
    if (_jointMatrices == nullptr) {
        _jointMatrices = TextureBuffer::Create("jointMatrices", GL_RGBA32F, BufferHelper::CreateAccessor<glm::mat4>(GetComponent<MeshSkin>()->Joints().size(), GL_TEXTURE_BUFFER));
        skinChanged = true;
        debugLog(Name() + " : Create Skin");
    }
    for (auto index = 0u; index < GetComponent<MeshSkin>()->Joints().size(); ++index) {
        const auto joint(GetComponent<MeshSkin>()->Joints().at(index));
        auto jointMatrix = glm::inverse(transform->Parent()->WorldTransformMatrix()) * joint.lock()->GetComponent<Transform>()->WorldTransformMatrix() * BufferHelper::Get<glm::mat4>(GetComponent<MeshSkin>()->InverseBindMatrices(), index);
        if (jointMatrix != BufferHelper::Get<glm::mat4>(_jointMatrices->Accessor(), index))
            skinChanged = true;
        BufferHelper::Set(_jointMatrices->Accessor(), index, jointMatrix);
    }
    SetNeedsFixedUpdateGPU(NeedsFixedUpdateGPU() || skinChanged);
}

std::shared_ptr<BufferAccessor> Mesh::Weights() const
{
    return _weights;
}

void Mesh::SetWeights(std::shared_ptr<BufferAccessor> weights)
{
    _weights = weights;
}
