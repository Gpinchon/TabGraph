/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:25
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:50:03
*/
#pragma once

#include "Renderer/SceneRenderer.hpp"
#include "Light/LightProbe.hpp"

#include <glm/mat4x4.hpp>
#include <array>
#include <memory>
#include <vector>
#include <map>
#include <set>

class Surface;
class Node;
class LightProbe;
class Scene;

namespace Renderer {
struct Options;
class SceneRenderer {
public:
    SceneRenderer(Scene&);
    SceneRenderer(SceneRenderer&) = delete;
    void OnFrameBegin(const Renderer::Options& options);
    void Render(const Renderer::Options& options, const glm::mat4& rootMatrix);
    void OnFrameEnd(const Renderer::Options& options);
    LightProbe& GetClosestLightProbe(const glm::vec3& position);

private:
    Scene& _scene;
    struct SurfaceState {
        glm::mat4 transform;
        glm::mat4 prevTransform;
        std::shared_ptr<Node> node;
    };
    void _UpdateRenderList(std::shared_ptr<Node> root);
    std::map<std::weak_ptr<Surface>, std::vector<SurfaceState>, std::owner_less<>> _renderList;
    std::map<std::weak_ptr<Node>, glm::mat4, std::owner_less<>> _nodeLastTransform;
    std::set<std::weak_ptr<Node>, std::owner_less<>> _nodesToKeep;
    LightProbeGroup _lightProbeGroup{ 1 };
    float _fixedDelta{ 0 };
    float _lightProbeDelta{ 0 };
};
};
