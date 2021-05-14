/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:35
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-12 18:39:43
*/

#include "Driver/OpenGL/Renderer/SceneRenderer.hpp"
#include "Light/Light.hpp"
#include "Light/LightProbe.hpp"
#include "Surface/Mesh.hpp"
#include "Node.hpp"
#include "Renderer/Light/LightRenderer.hpp"
#include "Renderer/Surface/SurfaceRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Framebuffer.hpp"
#include "Texture/Texture2D.hpp"

namespace Renderer {
SceneRenderer::SceneRenderer(Scene& scene)
    : _scene(scene)
{
}

void SceneRenderer::OnFrameBegin(uint32_t frameNbr, float delta)
{
    for (const auto& node : _scene.GetComponents<Node>())
        _UpdateRenderList(node);
    for (auto& transform : _nodeLastTransform) {
        if (_nodesToKeep.count(transform.first) == 0)
            _nodeLastTransform.erase(transform.first);
    }
    for (const auto& meshItr : _renderList) {
        auto meshPtr { meshItr.first.lock() };
        Renderer::OnFrameBegin(meshPtr, frameNbr, delta);
    }
    if (frameNbr % 5 == 0)
        return;
    for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
        lightProbe.GetReflectionBuffer()->Load();
        for (auto& sh : lightProbe.GetDiffuseSH())
            sh = glm::vec3(0);
    }
    bool first = true;
    glDisable(GL_BLEND);
    for (const auto& light : _scene.GetComponents<Light>()) {
        for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
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
    for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
        lightProbe.GetReflectionBuffer()->GetColorBuffer(0)->GenerateMipmap();
    }
}

void SceneRenderer::Render(const ::Renderer::Options& options, const glm::mat4& rootMatrix)
{
    for (const auto& meshItr : _renderList) {
        auto meshPtr { meshItr.first.lock() };
        for (const auto& meshState : meshItr.second) {
            Renderer::Render(meshPtr, options, meshState.transform, meshState.prevTransform);
        }
    }
}

void SceneRenderer::OnFrameEnd(uint32_t frameNbr, float delta)
{
    for (const auto& meshItr : _renderList) {
        auto meshPtr { meshItr.first.lock() };
        Renderer::OnFrameEnd(meshPtr, frameNbr, delta);
    }
    _renderList.clear();
}

LightProbe& SceneRenderer::GetClosestLightProbe(const glm::vec3& position)
{
    LightProbe* closestLightProbe = nullptr;
    float shortestDistance = std::numeric_limits<float>::max();
    for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
        auto distance { glm::distance(position, lightProbe.GetAbsolutePosition()) };
        if (shortestDistance > distance) {
            shortestDistance = distance;
            closestLightProbe = &lightProbe;
        }
    }
    return *closestLightProbe;
}

void SceneRenderer::_UpdateRenderList(std::shared_ptr<Node> root)
{
    _nodesToKeep.insert(root);
    for (auto& mesh : root->GetComponents<Mesh>()) {
        _renderList[mesh].push_back({ root->WorldTransformMatrix(),
            _nodeLastTransform[root],
            root });
        _nodeLastTransform[root] = root->WorldTransformMatrix();
    }
    for (auto& node : root->GetChildren()) {
        _UpdateRenderList(node);
    }
}
};
