/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:35
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 08:05:29
*/

#include "Driver/OpenGL/Renderer/SceneRenderer.hpp"
#include "Mesh/Mesh.hpp"
#include "Node.hpp"
#include "Renderer/MeshRenderer.hpp"
#include "Renderer/Light/LightRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Light/LightProbe.hpp"
#include "Light/Light.hpp"
#include "Framebuffer.hpp"
#include "Texture/Texture2D.hpp"

static LightProbeGroup lightProbeGroup(1);

namespace Renderer {
void SceneRenderer::Impl::OnFrameBegin(Scene& scene, uint32_t frameNbr, float delta)
{
    for (const auto& node : scene.GetComponents<Node>())
        _UpdateRenderList(node);
    for (auto& transform : _nodeLastTransform) {
        if (_nodesToKeep.count(transform.first) == 0)
            _nodeLastTransform.erase(transform.first);
    }
    for (const auto& meshItr : _renderList) {
        auto meshPtr{ meshItr.first.lock() };
        Renderer::OnFrameBegin(meshPtr, frameNbr, delta);
    }
    if (frameNbr % 5 == 0)
        return;
    for (auto& lightProbe : lightProbeGroup.GetLightProbes()) {
        lightProbe.GetReflectionBuffer()->Load();
        for (auto& sh : lightProbe.GetDiffuseSH())
            sh = glm::vec3(0);
    }
    bool first = true;
    glDisable(GL_BLEND);
    for (const auto& light : scene.GetComponents<Light>()) {
        for (auto& lightProbe : lightProbeGroup.GetLightProbes()) {
            Renderer::UpdateLightProbe(light, lightProbe);
            //light->DrawProbe(lightProbe);
        }
        if (first) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glBlendEquation(GL_FUNC_ADD);
            first = false;
        }
    }
    for (auto& lightProbe : lightProbeGroup.GetLightProbes()) {
        lightProbe.GetReflectionBuffer()->GetColorBuffer(0)->GenerateMipmap();
    }
}

void SceneRenderer::Impl::Render(Scene& scene, const ::Renderer::Options& options, const glm::mat4& rootMatrix)
{
    for (const auto& meshItr : _renderList) {
        auto meshPtr{ meshItr.first.lock() };
        for (const auto& meshState : meshItr.second) {
            Renderer::Render(meshPtr, options, meshState.transform, meshState.prevTransform);
        }
    }
}

void SceneRenderer::Impl::OnFrameEnd(Scene& scene, uint32_t frameNbr, float delta)
{
    for (const auto& meshItr : _renderList) {
        auto meshPtr{ meshItr.first.lock() };
        Renderer::OnFrameEnd(meshPtr, frameNbr, delta);
    }
    _renderList.clear();
}

LightProbe& SceneRenderer::Impl::GetClosestLightProbe(const glm::vec3& position)
{
    LightProbe* closestLightProbe = nullptr;
    float shortestDistance = std::numeric_limits<float>::max();
    for (auto& lightProbe : lightProbeGroup.GetLightProbes()) {
        auto distance{ glm::distance(position, lightProbe.GetAbsolutePosition()) };
        if (shortestDistance > distance) {
            shortestDistance = distance;
            closestLightProbe = &lightProbe;
        }
    }
    return *closestLightProbe;
}

void SceneRenderer::Impl::_UpdateRenderList(std::shared_ptr<Node> root)
{
    _nodesToKeep.insert(root);
    for (auto& mesh : root->GetComponents<Mesh>()) {
        _renderList[mesh].push_back({
            root->WorldTransformMatrix(),
            _nodeLastTransform[root],
            root
        });
        _nodeLastTransform[root] = root->WorldTransformMatrix();
    }
    for (auto& node : root->GetChildren()) {
        _UpdateRenderList(node);
    }
}
};
