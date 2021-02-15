/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:20
*/

#include "Mesh/Mesh.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Camera/Camera.hpp" // for Camera
#include "Debug.hpp"
#include "Material/Material.hpp" // for Material
#include "Mesh/MeshSkin.hpp"
#include "Physics/BoundingAABB.hpp" // for BoundingAABB
#include "Physics/BoundingElement.hpp" // for BoundingElement
#include "Render.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Texture2D.hpp"
#include "Texture/TextureBuffer.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>

size_t meshNbr(0);

Mesh::Mesh(const std::string& name)
    : Component(name)
{
    meshNbr++;
}

Mesh::Mesh(const Mesh& other) : Component(other)
{
    meshNbr++;
}

Mesh::Mesh()
    : Mesh("Mesh_" + std::to_string(meshNbr))
{
    meshNbr++;
}

void Mesh::AddGeometry(std::shared_ptr<Geometry> group)
{
    AddComponent(group);
}

#include "Framebuffer.hpp"
#include "Render.hpp"
#include "Window.hpp"

#include "Config.hpp"

bool Mesh::Draw(const glm::mat4& parentTransform, const glm::mat4& parentPrevTransform, const RenderPass& pass, RenderMod mod)
{
    auto currentCamera(Scene::Current() ? Scene::Current()->CurrentCamera() : nullptr);
    auto transformMatrix = parentTransform * GetGeometryTransform();
    auto prevTransform = parentPrevTransform * _prevTransformMatrix;

    bool ret = false;
    auto normal_matrix = glm::inverseTranspose(glm::mat3(transformMatrix));

    Load();
    std::shared_ptr<Shader> last_shader;
    auto skinned{ HasComponentOfType<MeshSkin>() };
    auto jointsIndex = _jointMatricesIndex;
    auto prevJointsIndex = (_jointMatricesIndex - 1) % _jointMatrices.size();
    auto fastTransparency{ Config::Global().Get("FastTransparency", 1) };
    for (auto vg : Geometrys()) {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material) {
            errorLog("Error : Invalid Material Index while rendering Mesh");
            continue;
        }
        auto isTransparent(material->GetOpacityMode() == Material::OpacityModeValue::Blend);
        //if (mod == RenderMod::RenderOpaque && (isTransparent && material->GetOpacity() < 1))
        if (mod == RenderMod::RenderOpaque && isTransparent)
            continue;
        else if (mod == RenderMod::RenderTransparent && !isTransparent)
            continue;
        std::shared_ptr<Shader> shader;
        if (pass == RenderPass::Geometry)
            shader = material->GeometryShader();
        else if (pass == RenderPass::Material)
            shader = material->MaterialShader();
        if (nullptr == shader)
            continue;
        material->Bind();
        //std::cout << unsigned(vg->Id()) << std::endl;
        shader->SetUniform("DrawID", unsigned(vg->GetId()));
        if (last_shader != shader) {
            //last_shader->use(false);
            if (pass == RenderPass::Material) {
                if (isTransparent&& fastTransparency)
                    shader->SetDefine("FASTTRANSPARENCY");
                shader->SetTexture("DiffuseTexture", Render::LightBuffer()->attachement(0));
                //shader->SetTexture("SpecularTexture", Render::LightBuffer()->attachement(1));
                shader->SetTexture("ReflectionTexture", Render::LightBuffer()->attachement(1));
                shader->SetTexture("AOTexture", Render::GeometryBuffer()->attachement(3));
                shader->SetTexture("NormalTexture", Render::GeometryBuffer()->attachement(4));
                shader->SetTexture("IDTexture", Render::GeometryBuffer()->attachement(5));
                shader->SetUniform("RenderPass", int(mod));
                if (material->GetOpacityMode() == Material::OpacityModeValue::Blend)
                    shader->SetTexture("OpaqueDepthTexture", Render::OpaqueBuffer()->depth());
            }
            shader->SetUniform("PrevMatrix.Model", prevTransform);
            shader->SetUniform("Matrix.Model", transformMatrix);
            shader->SetUniform("Matrix.Normal", normal_matrix);
            shader->SetTexture("Joints", skinned ? _jointMatrices.at(jointsIndex) : nullptr);
            shader->SetTexture("PrevJoints", skinned ? _jointMatrices.at(prevJointsIndex) : nullptr);
            shader->SetUniform("Skinned", skinned);
            last_shader = shader;
        }
        if (material->GetDoubleSided())
            glDisable(GL_CULL_FACE);
        else
            glEnable(GL_CULL_FACE);
        shader->use();
        ret |= vg->Draw();
        if (material->GetDoubleSided())
            glEnable(GL_CULL_FACE);
    }
    last_shader->use(false);
    if (pass == RenderPass::Geometry) {
        _prevTransformMatrix = GetGeometryTransform();
        if (skinned) {
            glDeleteSync(_drawSync[prevJointsIndex]);
            _drawSync[prevJointsIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glDeleteSync(_drawSync[jointsIndex]);
            _drawSync[jointsIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }
    }
    return ret;
}

bool Mesh::DrawDepth(const glm::mat4& parentTransform, RenderMod mod)
{
    auto currentCamera(Scene::Current() ? Scene::Current()->CurrentCamera() : nullptr);
    auto transformMatrix = parentTransform * GetGeometryTransform();

    bool ret = false;
    auto normal_matrix = glm::inverseTranspose(glm::mat3(transformMatrix));

    Load();
    std::shared_ptr<Shader> last_shader;
    auto skinned{ HasComponentOfType<MeshSkin>() };
    auto jointsIndex = _jointMatricesIndex;
    for (auto vg : Geometrys()) {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material) {
            errorLog("Error : Invalid Material Index while rendering Mesh");
            continue;
        }
        auto isTransparent(material->GetOpacityMode() == Material::OpacityModeValue::Blend);
        //if (mod == RenderMod::RenderOpaque && (isTransparent && material->GetOpacity() < 1))
        if (mod == RenderMod::RenderOpaque && isTransparent)
            continue;
        else if (mod == RenderMod::RenderTransparent && !isTransparent)
            continue;
        auto shader{ material->GeometryShader() };
        material->Bind();
        shader->SetUniform("DrawID", unsigned(vg->GetId()));
        if (last_shader != shader) {
            shader->SetUniform("Matrix.Model", transformMatrix);
            shader->SetUniform("Matrix.Normal", normal_matrix);
            shader->SetTexture("Joints", skinned ? _jointMatrices.at(jointsIndex) : nullptr);
            shader->SetUniform("Skinned", skinned);
            last_shader = shader;
        }
        if (material->GetDoubleSided())
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
    AddComponent(material);
    //_materials.push_back(material);
}

void Mesh::RemoveMaterial(std::shared_ptr<Material> material)
{
    //SetMaterial(nullptr, GetMaterialIndex(material));
    RemoveComponent(material);
}

std::shared_ptr<Material> Mesh::GetMaterial(uint32_t index)
{
    return GetComponent<Material>(index);
    //return index >= _materials.size() ? nullptr : _materials.at(index).lock();
}

int64_t Mesh::GetMaterialIndex(std::shared_ptr<Material> mtl)
{
    return GetComponentIndex(mtl);
    /*for (auto i(0u); i < _materials.size(); ++i) {
        if (_materials.at(i).lock() == mtl)
            return i;
    }
    return -1;*/
}

int64_t Mesh::GetMaterialIndex(const std::string& name)
{
    auto material = GetComponentByName<Material>(name);
    return GetMaterialIndex(material);
}

void Mesh::Load()
{
    if (GetLoaded())
        return;
    debugLog(GetName());
    if (HasComponentOfType<MeshSkin>()) {
        for (auto i = 0; i < _jointMatrices.size(); ++i) {
            if (_jointMatrices.at(i) != nullptr)
                continue;
            auto bufferAccessor = Component::Create<BufferAccessor>(BufferAccessor::ComponentType::Float32, BufferAccessor::Type::Mat4, GetComponent<MeshSkin>()->Joints().size());
            bufferAccessor->GetBufferView()->SetType(BufferView::Type::TextureBuffer);
            bufferAccessor->GetBufferView()->SetMode(BufferView::Mode::Persistent);
            bufferAccessor->GetBufferView()->SetPersistentMappingMode(BufferView::MappingMode::WriteOnly);
            _jointMatrices.at(i) = Component::Create<TextureBuffer>(Pixel::SizedFormat::Float32_RGBA, bufferAccessor);
            _jointMatrices.at(i)->Load();
        }
        debugLog(GetName() + " : Create Skin");
    }
    _SetLoaded(true);
}

void Mesh::UpdateSkin(const glm::mat4& parentTransform)
{
    if (!HasComponentOfType<MeshSkin>())
        return;
    Load();
    _jointMatricesIndex = (_jointMatricesIndex + 1) % _jointMatrices.size();
    auto meshSkin{ GetComponent<MeshSkin>() };
    auto invMatrix = glm::inverse(parentTransform);
    if (glIsSync(_drawSync.at(_jointMatricesIndex))) {
        while (glClientWaitSync(_drawSync.at(_jointMatricesIndex), 0, 1) == GL_TIMEOUT_EXPIRED) {}
        glDeleteSync(_drawSync.at(_jointMatricesIndex));
        _drawSync.at(_jointMatricesIndex) = nullptr;
    }
    const auto joints = meshSkin->Joints();
    auto jointMatricesAccessor{ _jointMatrices.at(_jointMatricesIndex)->Accessor() };
    auto inverseBindMatrices{ meshSkin->InverseBindMatrices() };
    //jointMatricesAccessor->GetBufferView()->MapRange(
    //    BufferView::MappingMode::WriteOnly,
    //    jointMatricesAccessor->GetByteOffset(),
    //    jointMatricesAccessor->GetCount() * jointMatricesAccessor->GetTypeOctetsSize()
    //);
    for (auto index = 0u; index < joints.size(); ++index) {
        const auto jointMatrixIndex{ index };
        const auto &joint(joints.at(index));
        auto jointMatrix = invMatrix * joint->WorldTransformMatrix() * inverseBindMatrices->Get<glm::mat4>(index);
        jointMatricesAccessor->Set(jointMatrix, jointMatrixIndex);
    }
    //jointMatricesAccessor->GetBufferView()->Unmap();
}

std::shared_ptr<BufferAccessor> Mesh::Weights() const
{
    return GetComponent<BufferAccessor>();
}

void Mesh::SetWeights(std::shared_ptr<BufferAccessor> weights)
{
    SetComponent(weights);
}
