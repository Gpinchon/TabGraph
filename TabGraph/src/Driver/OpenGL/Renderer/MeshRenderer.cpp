/*
* @Author: gpinchon
* @Date:   2021-03-23 13:40:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-01 18:59:05
*/

#include "Driver/OpenGL/Renderer/MeshRenderer.hpp"
#include "Config.hpp"
#include "Material/Material.hpp"
#include "Mesh/Geometry.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/MeshSkin.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Shader/Program.hpp"
#include "Texture/TextureBuffer.hpp"
#include "Scene/Scene.hpp"
#include "Renderer/SceneRenderer.hpp"
#include "Light/LightProbe.hpp"
#include "Framebuffer.hpp"
#include "Texture/Texture2D.hpp"

namespace Renderer {
void MeshRenderer::Impl::Load(Mesh& mesh)
{
    if (_loaded)
        return;
    if (mesh.HasComponentOfType<MeshSkin>()) {
        for (auto i = 0; i < _jointMatrices.size(); ++i) {
            if (_jointMatrices.at(i) != nullptr)
                continue;
            auto bufferAccessor = Component::Create<BufferAccessor>(
                BufferAccessor::ComponentType::Float32,
                BufferAccessor::Type::Mat4,
                mesh.GetComponent<MeshSkin>()->Joints().size());
            bufferAccessor->GetBufferView()->SetType(BufferView::Type::TextureBuffer);
            bufferAccessor->GetBufferView()->SetMode(BufferView::Mode::Persistent);
            bufferAccessor->GetBufferView()->SetPersistentMappingMode(BufferView::MappingMode::WriteOnly);
            _jointMatrices.at(i) = Component::Create<TextureBuffer>(Pixel::SizedFormat::Float32_RGBA, bufferAccessor);
            _jointMatrices.at(i)->Load();
        }
        debugLog(mesh.GetName() + " : Create Skin");
    }
    _loaded = true;
}

void MeshRenderer::Impl::OnFrameBegin(Mesh& mesh, uint32_t frameNbr, float delta)
{
    mesh.Load();
    Load(mesh);
    _jointMatricesIndex = (_jointMatricesIndex + 1) % _jointMatrices.size();
    auto meshSkin { mesh.GetComponent<MeshSkin>() };
    if (meshSkin == nullptr)
        return;
    //auto invMatrix = glm::inverse(parentTransform);
    if (glIsSync(_drawSync.at(_jointMatricesIndex))) {
        while (glClientWaitSync(_drawSync.at(_jointMatricesIndex), 0, 1) == GL_TIMEOUT_EXPIRED) { }
        glDeleteSync(_drawSync.at(_jointMatricesIndex));
        _drawSync.at(_jointMatricesIndex) = nullptr;
    }
    const auto joints = meshSkin->Joints();
    auto jointMatricesAccessor { _jointMatrices.at(_jointMatricesIndex)->Accessor() };
    auto inverseBindMatrices { meshSkin->InverseBindMatrices() };
    //jointMatricesAccessor->GetBufferView()->MapRange(
    //    BufferView::MappingMode::WriteOnly,
    //    jointMatricesAccessor->GetByteOffset(),
    //    jointMatricesAccessor->GetCount() * jointMatricesAccessor->GetTypeOctetsSize()
    //);
    for (auto index = 0u; index < joints.size(); ++index) {
        const auto jointMatrixIndex { index };
        const auto& joint(joints.at(index));
        auto jointMatrix = joint->WorldTransformMatrix() * inverseBindMatrices->Get<glm::mat4>(index);
        jointMatricesAccessor->Set(jointMatrix, jointMatrixIndex);
    }
    //jointMatricesAccessor->GetBufferView()->Unmap();
}

void MeshRenderer::Impl::Render(
    Mesh& mesh,
    const ::Renderer::Options& options,
    const glm::mat4& parentTransform,
    const glm::mat4& parentLastTransform)
{
    auto currentCamera(options.camera);
    auto transformMatrix = parentTransform * mesh.GetGeometryTransform();
    auto prevTransform = parentLastTransform * _prevTransformMatrix;
    auto skinTransform = glm::inverse(parentTransform);
    auto skinPrevTransform = glm::inverse(parentLastTransform);
    auto &lightProbe = options.scene->GetRenderer().GetClosestLightProbe(transformMatrix * glm::vec4(0, 0, 0, 1));

    auto normal_matrix = glm::inverseTranspose(glm::mat3(transformMatrix));

    std::shared_ptr<Shader::Program> last_shader;
    auto skinned { mesh.HasComponentOfType<MeshSkin>() };
    auto jointsIndex = _jointMatricesIndex;
    auto prevJointsIndex = (_jointMatricesIndex - 1) % _jointMatrices.size();
    auto fastTransparency { Config::Global().Get("FastTransparency", 1) };
    for (const auto &vg : mesh.GetGeometries()) {
        if (nullptr == vg)
            continue;
        const auto& material{ mesh.GetGeometryMaterial(vg) };
        if (nullptr == material) {
            errorLog("Error : Invalid Material Index while rendering Mesh");
            continue;
        }
        const auto isTransparent(material->GetOpacityMode() == Material::OpacityMode::Blend);
        if (options.mode == ::Renderer::Options::Mode::Opaque && isTransparent)
            continue;
        else if (options.mode == ::Renderer::Options::Mode::Transparent && !isTransparent)
            continue;
        std::shared_ptr<Shader::Program> shader { material->GetShader(options.pass) };
        if (nullptr == shader)
            continue;
        material->Bind(options.pass);
        shader->Use()
            .SetUniform("DrawID", unsigned(vg->GetId()))
            .SetUniform("PrevMatrix.Model", prevTransform)
            .SetUniform("Matrix.Model", transformMatrix)
            .SetUniform("Matrix.Normal", normal_matrix)
            .SetUniform("SkinTransform", skinTransform)
            .SetUniform("SkinPrevTransform", skinPrevTransform)
            .SetTexture("Joints", skinned ? _jointMatrices.at(jointsIndex) : nullptr)
            .SetTexture("PrevJoints", skinned ? _jointMatrices.at(prevJointsIndex) : nullptr)
            .SetUniform("Skinned", skinned);
        if (options.pass == Options::Pass::ForwardOpaque || options.pass == Options::Pass::ForwardTransparent)
        {
            shader->SetTexture("BRDFLUT", Renderer::DefaultBRDFLUT());
            shader->SetUniform("SH[0]", lightProbe.GetDiffuseSH().data(), lightProbe.GetDiffuseSH().size());
            shader->SetTexture("ReflectionMap", lightProbe.GetReflectionBuffer()->GetColorBuffer(0));
        }
        if (last_shader != shader) {
            //last_shader->use(false);
            //if (options.pass == Renderer::Pass::Material) {
            //    if (isTransparent && fastTransparency)
            //        shader->SetDefine("FASTTRANSPARENCY");
            //    shader->SetTexture("DiffuseTexture", Renderer::LightBuffer()->attachement(0));
            //    //shader->SetTexture("SpecularTexture", Renderer::LightBuffer()->attachement(1));
            //    shader->SetTexture("ReflectionTexture", Renderer::LightBuffer()->attachement(1));
            //    shader->SetTexture("AOTexture", Renderer::GeometryBuffer()->attachement(3));
            //    shader->SetTexture("NormalTexture", Renderer::GeometryBuffer()->attachement(4));
            //    shader->SetTexture("IDTexture", Renderer::GeometryBuffer()->attachement(5));
            //    shader->SetUniform("RenderPass", int(mod));
            //    if (isTransparent)
            //        shader->SetTexture("OpaqueDepthTexture", Renderer::OpaqueBuffer()->depth());
            //}
            last_shader = shader;
        }
        Renderer::Render(vg, material->GetDoubleSided());
    }
    last_shader->Done();
}

void MeshRenderer::Impl::OnFrameEnd(Mesh& mesh, uint32_t frameNbr, float delta)
{
    auto skinned { mesh.HasComponentOfType<MeshSkin>() };
    auto jointsIndex = _jointMatricesIndex;
    auto prevJointsIndex = (_jointMatricesIndex - 1) % _jointMatrices.size();
    _prevTransformMatrix = mesh.GetGeometryTransform();
    if (skinned) {
        glDeleteSync(_drawSync[prevJointsIndex]);
        _drawSync[prevJointsIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glDeleteSync(_drawSync[jointsIndex]);
        _drawSync[jointsIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
}
};
