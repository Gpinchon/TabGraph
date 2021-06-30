/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:25
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:50:03
*/
#pragma once

#include <Renderer/SceneRenderer.hpp>
#include <Light/LightProbe.hpp>

#include <glm/mat4x4.hpp>
#include <array>
#include <memory>
#include <vector>
#include <map>
#include <set>

namespace TabGraph {
class Shape;
namespace Nodes {
class Node;
class Scene;
};
class LightProbe;
};

namespace TabGraph::Renderer {
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
    struct ShapeState {
        glm::mat4 transform;
        std::weak_ptr<Shape> surface;
    };
    void _UpdateRenderList(std::shared_ptr<Node> root);
    std::vector<ShapeState> _renderList;
    std::map<std::weak_ptr<Node>, glm::mat4, std::owner_less<>> _nodeLastTransform;
    std::set<std::weak_ptr<Node>, std::owner_less<>> _nodesToKeep;
    LightProbeGroup _lightProbeGroup{ 1 };
    float _fixedDelta{ 0 };
    float _lightProbeDelta{ 0 };
};
};
