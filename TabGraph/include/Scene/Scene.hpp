/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:58:35
*/
#pragma once

#include <Common.hpp>
#include <Object.hpp>

#include <glm/vec3.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>

class PhysicsImpostor;
class Camera;
class Light;
class Animation;
class BoundingAABB;
class RigidBody;
class Skybox;
class PhysicsEngine;
class Node;

namespace Renderer {
class SceneRenderer;
struct SceneRendererDeleter
{
    void operator()(Renderer::SceneRenderer*);
};
};

/**
 * @brief scene container
 */
class Scene : public Object {
public:
    Renderer::SceneRenderer& GetRenderer();
    Scene(const std::string& name);
    Scene(const Scene& other);
    ~Scene() = default;
    void Remove(std::shared_ptr<Node>);
    void Add(std::shared_ptr<Animation>);
    void Add(std::shared_ptr<RigidBody>);
    std::shared_ptr<BoundingAABB> GetLimits() const;
    inline auto GetRootNode() const {
        return _rootNode;
    }
    inline auto& GetAnimations() const {
        return _animations;
    }
    inline auto GetCamera() const {
        return _camera.lock();
    }
    inline auto GetSkybox() const {
        return _skybox.lock();
    }
    void SetCamera(std::shared_ptr<Camera> camera);
    void SetSkybox(std::shared_ptr<Skybox> skybox);

    inline glm::vec3 Up() const {
        return _up;
    }
    inline void SetUp(const glm::vec3& up) {
        _up = up;
    }

private:
    std::shared_ptr<Node> _rootNode;
    std::shared_ptr<PhysicsEngine> _physicsEngine;
    glm::vec3 _up { Common::Up() };
    std::shared_ptr<BoundingAABB> _aabb { nullptr };
    std::unique_ptr<Renderer::SceneRenderer, Renderer::SceneRendererDeleter> _renderer;
    std::weak_ptr<Camera> _camera;
    std::weak_ptr<Skybox> _skybox;
    std::set<std::shared_ptr<Animation>> _animations;
};