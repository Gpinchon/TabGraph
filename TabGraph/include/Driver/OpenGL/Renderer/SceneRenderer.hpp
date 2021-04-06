/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:25
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:50:03
*/
#pragma once

#include "Renderer/SceneRenderer.hpp"

#include <array>
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>
#include <map>

class Mesh;
class Node;
class LightProbe;

namespace Renderer {
class SceneRenderer::Impl {
public:
    void OnFrameBegin(Scene& scene, uint32_t frameNbr, float delta);
    void Render(Scene& scene, const ::Renderer::Options& options, const glm::mat4& rootMatrix);
    void OnFrameEnd(Scene& scene, uint32_t frameNbr, float delta);
    LightProbe& GetClosestLightProbe(const glm::vec3& position);

private:
    struct MeshState {
        glm::mat4 transform;
        glm::mat4 prevTransform;
        std::shared_ptr<Node> node;
    };
    void _UpdateRenderList(std::shared_ptr<Node> root);
    std::map<std::shared_ptr<Mesh>, std::vector<MeshState>> _renderList;
    std::map<std::shared_ptr<Node>, glm::mat4> _nodeLastTransform;
    //std::vector<MeshState> _renderList;
    //std::set<std::shared_ptr<Mesh>> _shapesList;
};
};
