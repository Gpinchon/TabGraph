/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:58:35
*/
#pragma once

#include "Common.hpp"
#include "Component.hpp"
#include "Node.hpp"

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
class Scene : public Component {
public:
    Renderer::SceneRenderer& GetRenderer();
    Scene(const std::string& name);
    Scene(const Scene& other);
    static std::shared_ptr<Scene> Current();
    static void SetCurrent(std::shared_ptr<Scene>);
    std::shared_ptr<Camera> CurrentCamera() const;
    void SetCurrentCamera(std::shared_ptr<Camera>);
    ~Scene() = default;
    void AddRootNode(std::shared_ptr<Node>);
    void Remove(std::shared_ptr<Node>);
    void Add(std::shared_ptr<Node>);
    void Add(std::shared_ptr<Animation>);
    void Add(std::shared_ptr<RigidBody>);
    std::shared_ptr<BoundingAABB> GetLimits() const;
    std::shared_ptr<Skybox> GetSkybox() const;
    void SetSkybox(const std::shared_ptr<Skybox>& env);
    glm::vec3 Up() const;
    void SetUp(glm::vec3);
    Signal<std::shared_ptr<Skybox>> SkyboxChanged;

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<Scene>(*this);
    }
    glm::vec3 _up { Common::Up() };
    std::shared_ptr<BoundingAABB> _aabb { nullptr };
    int64_t _currentCamera { -1 };
    int64_t _skyboxIndex { -1 };
    std::unique_ptr<Renderer::SceneRenderer, Renderer::SceneRendererDeleter> _renderer;
};