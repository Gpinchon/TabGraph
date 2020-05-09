/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-10-10 09:52:02
*/

#include "Mesh.hpp"
#include "BoundingAABB.hpp" // for BoundingAABB
#include "BufferHelper.hpp"
#include "BoundingElement.hpp" // for BoundingElement
#include "Camera.hpp" // for Camera
#include "Debug.hpp"
#include "Material.hpp" // for Material
#include "Node.hpp" // for Node
#include "Scene.hpp"
#include "Shader.hpp" // for Shader
#include "Texture2D.hpp"
#include "TextureBuffer.hpp"
#include "Geometry.hpp" // for Geometry
#include "MeshSkin.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


Mesh::Mesh(const std::string &name)
    : Node(name)
{
}

std::shared_ptr<Mesh> Mesh::Create(std::shared_ptr<Mesh> otherMesh) /*static*/
{
    return std::shared_ptr<Mesh>(new Mesh(*otherMesh));
}

std::shared_ptr<Mesh> Mesh::Create(const std::string &name) /*static*/
{
    return std::shared_ptr<Mesh>(new Mesh(name));
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

void Mesh::Load()
{
    if (_loaded)
        return;
    for (auto vg : _Geometrys)
        vg->Load();
    if (_jointMatrices != nullptr)
        _jointMatrices->load();
}

bool Mesh::DrawDepth(RenderMod mod)
{
    auto currentCamera(Scene::Current() ? Scene::Current()->CurrentCamera() : nullptr);
    auto geometryTranslationMatrix(glm::translate(GeometryPosition()));
    auto geometryRotationMatrix(glm::mat4_cast(GeometryRotation()));
    auto geometryScaleMatrix(glm::scale(GeometryScale()));
    auto finalTranformMatrix(TransformMatrix() * geometryTranslationMatrix * geometryRotationMatrix * geometryScaleMatrix);

    bool ret = false;
    auto viewProjectionMatrix = currentCamera->ProjectionMatrix() * currentCamera->ViewMatrix();
    auto normal_matrix = glm::inverseTranspose(finalTranformMatrix);

    Load();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : _Geometrys)
    {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material)
            continue;
        auto isTransparent(material->Alpha() < 1);// || (material->TextureAlbedo() != nullptr && material->TextureAlbedo()->values_per_pixel() == 4));
        if (mod == RenderMod::RenderOpaque && isTransparent)
            continue;
        else if (mod == RenderMod::RenderTransparent && !isTransparent)
            continue;
        if (nullptr == material->depth_shader())
            continue;
        auto shader(material->depth_shader());
        material->Bind();
        if (last_shader != shader)
        {
            shader->SetUniform("Camera.Position", Scene::Current()->CurrentCamera()->Position());
            shader->SetUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
            shader->SetUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
            shader->SetUniform("Camera.Matrix.ViewProjection", viewProjectionMatrix);
            shader->SetUniform("Matrix.Model", finalTranformMatrix);
            shader->SetUniform("Matrix.Normal", normal_matrix);
            if (Skin() != nullptr) {
                shader->SetUniform("Joints", _jointMatrices, GL_TEXTURE11);
                shader->SetUniform("Skinned", true);
            }
            else {
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

bool Mesh::Draw(RenderMod mod)
{
    auto currentCamera(Scene::Current() ? Scene::Current()->CurrentCamera() : nullptr);
    auto geometryTranslationMatrix(glm::translate(GeometryPosition()));
    auto geometryRotationMatrix(glm::mat4_cast(GeometryRotation()));
    auto geometryScaleMatrix(glm::scale(GeometryScale()));
    auto finalTranformMatrix(TransformMatrix() * geometryTranslationMatrix * geometryRotationMatrix * geometryScaleMatrix);

    bool ret = false;
    auto viewProjectionMatrix = currentCamera->ProjectionMatrix() * currentCamera->ViewMatrix();
    auto normal_matrix = glm::inverseTranspose(finalTranformMatrix);

    //auto geometryTransform()

    Load();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : _Geometrys)
    {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material)
        {
            errorLog("Error : Invalid Material Index while rendering Mesh" + Name());
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
        if (last_shader != shader)
        {
            shader->SetUniform("Camera.Matrix.ViewProjection", viewProjectionMatrix);
            shader->SetUniform("Matrix.Model", finalTranformMatrix);
            shader->SetUniform("Matrix.Normal", normal_matrix);
            if (Skin() != nullptr) {
                shader->SetUniform("Joints", _jointMatrices, GL_TEXTURE11);
                shader->SetUniform("Skinned", true);
            }
            else {
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
    for (auto i(0u); i < _materials.size(); ++i)
    {
        if (_materials.at(i) == mtl)
            return i;
    }
    return -1;
}

int64_t Mesh::GetMaterialIndex(const std::string &name)
{
    for (auto i(0u); i < _materials.size(); ++i)
    {
        if (_materials.at(i)->Name() == name)
            return i;
    }
    return -1;
}

glm::vec3 Mesh::GeometryPosition() const
{
    return _geometryPosition;
}

void Mesh::SetGeometryPosition(glm::vec3 position)
{
    _geometryPosition = position;
    SetNeedsTranformUpdate(true);
}

glm::quat Mesh::GeometryRotation() const
{
    return _geometryRotation;
}

void Mesh::SetGeometryRotation(glm::quat rotation)
{
    _geometryRotation = rotation;
    SetNeedsTranformUpdate(true);
}

glm::vec3 Mesh::GeometryScale() const
{
    return _geometryScale;
}

void Mesh::SetGeometryScale(glm::vec3 scale)
{
    _geometryScale = scale;
    SetNeedsTranformUpdate(true);
}

void Mesh::FixedUpdate()
{
    UpdateSkin();
}

void Mesh::UpdateGPU()
{
    if (NeedsGPUUpdate() && _jointMatrices != nullptr)
        _jointMatrices->Accessor()->GetBufferView()->GetBuffer()->UpdateGPU();
    SetNeedsGPUUpdate(false);
}

void Mesh::UpdateSkin()
{
    if (Skin() == nullptr)
        return;
    bool skinChanged(true);
    for (auto index = 0u; index < Skin()->Joints().size(); ++index) {
        const auto joint(Skin()->Joints().at(index));
        auto jointMatrix =
            glm::inverse(Parent()->TransformMatrix()) *
            joint.lock()->TransformMatrix() *
            BufferHelper::Get<glm::mat4>(Skin()->InverseBindMatrices(), index);
        if (jointMatrix != BufferHelper::Get<glm::mat4>(_jointMatrices->Accessor(), index))
            skinChanged = true;
        BufferHelper::Set(_jointMatrices->Accessor(), index, jointMatrix);
    }
    SetNeedsGPUUpdate(NeedsGPUUpdate() || skinChanged);
}

std::shared_ptr<MeshSkin> Mesh::Skin() const
{
    return _skin;
}

void Mesh::SetSkin(std::shared_ptr<MeshSkin> skin)
{
    _skin = skin;
    _jointMatrices = TextureBuffer::Create("jointMatrices", GL_RGBA32F, BufferHelper::CreateAccessor<glm::mat4>(_skin->Joints().size(), GL_TEXTURE_BUFFER));
    SetNeedsGPUUpdate(true);
}

std::shared_ptr<BufferAccessor> Mesh::Weights() const
{
    return _weights;
}

void Mesh::SetWeights(std::shared_ptr<BufferAccessor> weights)
{
    _weights = weights;
}