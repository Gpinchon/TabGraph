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
#include "Node.hpp" // for Node
#include "Physics/BoundingAABB.hpp" // for BoundingAABB
#include "Physics/BoundingElement.hpp" // for BoundingElement
#include "Render.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Texture2D.hpp"
#include "Texture/TextureBuffer.hpp"
#include "Transform.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>

size_t meshNbr(0);

Mesh::Mesh(const std::string& name)
    : Component(name)
{
}

Mesh::Mesh()
    : Mesh("Mesh_" + std::to_string(meshNbr))
{
    Render::OnBeforeRender().ConnectMember(this, &Mesh::_UpdateGPU);
    meshNbr++;
}

void Mesh::AddGeometry(std::shared_ptr<Geometry> group)
{
    AddComponent(group);
}

#include "Framebuffer.hpp"
#include "Render.hpp"
#include "Window.hpp"

bool Mesh::DrawDepth(const std::shared_ptr<Transform>& transform, RenderMod mod)
{
    auto currentCamera(Scene::Current() ? Scene::Current()->CurrentCamera() : nullptr);
    std::shared_ptr<Transform> geometryTransform(HasComponentOfType<Transform>() ? GetComponent<Transform>() : nullptr);
    auto finalTranformMatrix(transform->WorldTransformMatrix() * (geometryTransform ? geometryTransform->WorldTransformMatrix() : glm::mat4(1.f)));

    bool ret = false;
    auto normal_matrix = glm::inverseTranspose(finalTranformMatrix);

    Load();
    std::shared_ptr<Shader> last_shader;
    for (auto vg : Geometrys()) {
        if (nullptr == vg)
            continue;
        auto material(GetMaterial(vg->MaterialIndex()));
        if (nullptr == material)
            continue;
        auto isTransparent(material->GetOpacityMode() == Material::OpacityModeValue::Blend);
        //if (mod == RenderMod::RenderOpaque && (isTransparent && material->GetOpacity() < 1))
        if (mod == RenderMod::RenderOpaque && isTransparent)
            continue;
        else if (mod == RenderMod::RenderTransparent && !isTransparent)
            continue;
        auto shader(material->GeometryShader());
        if (nullptr == shader)
            continue;
        material->Bind();
        if (last_shader != shader) {
            shader->SetUniform("DrawID", unsigned(vg->GetId()));
            shader->SetUniform("Matrix.Model", finalTranformMatrix);
            shader->SetUniform("Matrix.Normal", normal_matrix);
            shader->SetTexture("Joints", HasComponentOfType<TextureBuffer>() ? JointMatrices() : nullptr);
            shader->SetUniform("Skinned", HasComponentOfType<MeshSkin>());
            last_shader = shader;
        }
        shader->use();
        ret |= vg->Draw();
        shader->use(false);
    }
    return ret;
}

bool Mesh::Draw(const std::shared_ptr<Transform>& transform, const RenderPass& pass, RenderMod mod)
{
    auto currentCamera(Scene::Current() ? Scene::Current()->CurrentCamera() : nullptr);
    std::shared_ptr<Transform> geometryTransform(HasComponentOfType<Transform>() ? GetComponent<Transform>() : nullptr);
    _transformMatrix = (transform->WorldTransformMatrix() * (geometryTransform ? geometryTransform->WorldTransformMatrix() : glm::mat4(1.f)));

    bool ret = false;
    auto normal_matrix = glm::inverseTranspose(_transformMatrix);

    Load();
    std::shared_ptr<Shader> last_shader;
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
            shader->SetUniform("WindowSize", Window::size());
            if (pass == RenderPass::Material) {
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
            shader->SetUniform("PrevMatrix.Model", _prevTransformMatrix);
            shader->SetUniform("Matrix.Model", _transformMatrix);
            shader->SetUniform("Matrix.Normal", normal_matrix);
            shader->SetTexture("Joints", HasComponentOfType<TextureBuffer>() ? JointMatrices() : nullptr);
            shader->SetUniform("Skinned", HasComponentOfType<MeshSkin>());
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

std::shared_ptr<TextureBuffer> Mesh::JointMatrices() const
{
    return GetComponent<TextureBuffer>();
}

void Mesh::SetJointMatrices(const std::shared_ptr<TextureBuffer>& jointMatrices)
{
    RemoveComponent(jointMatrices);
    SetComponent(jointMatrices);
}

void Mesh::Load()
{
    if (GetLoaded())
        return;
    debugLog(GetName());
    if (HasComponentOfType<MeshSkin>() && !HasComponentOfType<TextureBuffer>()) {
        auto bufferAccessor = Component::Create<BufferAccessor>(BufferAccessor::ComponentType::Float32, BufferAccessor::Type::Mat4, GetComponent<MeshSkin>()->Joints().size()); //BufferHelper::CreateAccessor<glm::mat4>(GetComponent<MeshSkin>()->Joints().size(), GL_TEXTURE_BUFFER, false, GL_DYNAMIC_DRAW);
        bufferAccessor->GetBufferView()->SetType(BufferView::Type::PixelUnpack);
        bufferAccessor->GetBufferView()->SetMode(BufferView::Mode::Persistent);
        bufferAccessor->GetBufferView()->SetPersistentMappingMode(BufferView::MappingMode::WriteOnly);
        //SetJointMatrices(Component::Create<TextureBuffer>("jointMatrices", GL_RGBA32F, bufferAccessor));
        SetJointMatrices(Component::Create<TextureBuffer>(Pixel::SizedFormat::Float32_RGBA, bufferAccessor));
        debugLog(GetName() + " : Create Skin");
    }
    if (HasComponentOfType<TextureBuffer>())
        JointMatrices()->Load();
    _SetLoaded(true);
}

void Mesh::UpdateSkin(const std::shared_ptr<Transform>& transform)
{
    if (!HasComponentOfType<MeshSkin>())
        return;
    Load();
    auto meshSkin{ GetComponent<MeshSkin>() };
    auto invMatrix = glm::inverse(transform->GetParent()->WorldTransformMatrix());
    for (auto index = 0u; index < meshSkin->Joints().size(); ++index) {
        const auto joint(meshSkin->Joints().at(index));
        auto jointMatrix = invMatrix * joint->WorldTransformMatrix() * meshSkin->InverseBindMatrices()->Get<glm::mat4>(index);//BufferHelper::Get<glm::mat4>(GetComponent<MeshSkin>()->InverseBindMatrices(), index);
        JointMatrices()->Accessor()->Set<glm::mat4>(jointMatrix, index);
        //BufferHelper::Set(JointMatrices()->Accessor(), index, jointMatrix);
    }
}

std::shared_ptr<BufferAccessor> Mesh::Weights() const
{
    return GetComponent<BufferAccessor>();
}

void Mesh::SetWeights(std::shared_ptr<BufferAccessor> weights)
{
    SetComponent(weights);
}
