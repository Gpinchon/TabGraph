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
#include <Node.hpp>
#include <Surface/Surface.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Light/LightRenderer.hpp>
#include <Renderer/Surface/ShapeRenderer.hpp>
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
        _renderList.clear();
        auto fixedOptions = options;
        fixedOptions.delta = _fixedDelta;
        for (const auto& animation : _scene.GetAnimations()) {
            if (animation->Playing())
                animation->Advance(_fixedDelta);
        }
        _UpdateRenderList(_scene.GetRootNode());
        for (auto& transform : _nodeLastTransform) {
            if (_nodesToKeep.count(transform.first) == 0)
                _nodeLastTransform.erase(transform.first);
        }
        for (const auto& surfaceState : _renderList) {
            auto surface{ surfaceState.surface.lock() };
            Renderer::OnFrameBegin(surface, fixedOptions);
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
        for (const auto& light : _scene.GetRootNode()->GetComponents<Light>()) {
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
}

void SceneRenderer::Render(const ::Renderer::Options& options, const glm::mat4& rootMatrix)
{
    for (const auto& surfaceState : _renderList) {
        auto surface { surfaceState.surface.lock() };
        Renderer::Render(surface, options, surfaceState.transform, surfaceState.prevTransform);
    }
}

void SceneRenderer::OnFrameEnd(const ::Renderer::Options& options)
{
    if (_fixedDelta > 0.015) {
        for (const auto& surfaceItr : _renderList) {
            auto surface{ surfaceItr.surface.lock() };
            Renderer::OnFrameEnd(surface, options);
        }
        _fixedDelta = 0;
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
    //for (auto i = 0; i < root->GetSurfaceNbr(); ++i) {
    //    auto surface{ root->GetSurface(i) };
    for (const auto &surface : root->GetSurfaces()) {
        _renderList.push_back({
            root->WorldTransformMatrix(),
            _nodeLastTransform[root],
            surface
        });
        _nodeLastTransform[root] = root->WorldTransformMatrix();
    }
    for (auto& node : root->GetChildren()) {
        _UpdateRenderList(node);
    }
}
};
