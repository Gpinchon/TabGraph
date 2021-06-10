/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:35
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-12 18:39:43
*/

#include <Animation/Animation.hpp>
#include <Driver/OpenGL/Renderer/SceneRenderer.hpp>
#include <Light/Light.hpp>
#include <Light/LightProbe.hpp>
#include <Surface/Mesh.hpp>
#include <Node.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Light/LightRenderer.hpp>
#include <Renderer/Surface/SurfaceRenderer.hpp>
#include <Scene/Scene.hpp>
#include <Texture/Framebuffer.hpp>
#include <Texture/Texture2D.hpp>

#include <GL/glew.h>

namespace Renderer {
SceneRenderer::SceneRenderer(Scene& scene)
    : _scene(scene)
{
}

void SceneRenderer::OnFrameBegin(const Renderer::Options& options)
{
    _lightProbeDelta += options.delta;
    _fixedDelta += options.delta;
    if (_fixedDelta > 0.015) {
        _fixedDelta = 0;
        _renderList.clear();
        for (const auto& animation : _scene.GetComponents<Animation>()) {
            if (animation->Playing())
                animation->Advance(options.delta);
        }
        for (const auto& node : _scene.GetComponents<Node>())
            _UpdateRenderList(node);
        for (auto& transform : _nodeLastTransform) {
            if (_nodesToKeep.count(transform.first) == 0)
                _nodeLastTransform.erase(transform.first);
        }
    }
    if (_lightProbeDelta > 0.032) {
        _lightProbeDelta = 0;
        for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
            for (auto& sh : lightProbe.GetDiffuseSH())
                sh = glm::vec3(0);
        }
        bool first = true;
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        for (const auto& light : _scene.GetComponents<Light>()) {
            for (auto& lightProbe : _lightProbeGroup.GetLightProbes()) {
                Renderer::UpdateLightProbe(light, options, lightProbe);
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
    for (const auto& meshItr : _renderList) {
        auto meshPtr { meshItr.first.lock() };
        Renderer::OnFrameBegin(meshPtr, options);
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

void SceneRenderer::OnFrameEnd(const ::Renderer::Options& options)
{
    for (const auto& meshItr : _renderList) {
        auto meshPtr { meshItr.first.lock() };
        Renderer::OnFrameEnd(meshPtr, options);
    }
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
