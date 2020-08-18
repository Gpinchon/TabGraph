/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 20:07:07
*/
#pragma once

#include "Common.hpp"
#include "Component.hpp"
#include "Physics/PhysicsEngine.hpp"

#include <glm/vec3.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>

class PhysicsImpostor;
class Camera;
class Light;
class Node;
class Animation;
class AABB;
class RigidBody;
enum class RenderMod;

/**
 * @brief scene container
 */
struct Scene : public Component {
    static std::shared_ptr<Scene> Create(const std::string& name);
    static std::shared_ptr<Scene> Current();
    static void SetCurrent(std::shared_ptr<Scene>);
    std::shared_ptr<Camera> CurrentCamera() const;
    void SetCurrentCamera(std::shared_ptr<Camera>);
    ~Scene() = default;
    std::string Name() const;
    void SetName(const std::string& name);
    void AddRootNode(std::shared_ptr<Node>);
    void Add(std::shared_ptr<Node>);
    void Add(std::shared_ptr<Animation>);
    void Add(std::shared_ptr<RigidBody>);
    virtual void PhysicsUpdate(float delta);
    virtual void Render(const RenderMod&);
    virtual void RenderDepth(const RenderMod&);
    std::shared_ptr<AABB> GetLimits() const;
    std::shared_ptr<Node> GetNode(std::shared_ptr<Node>) const;
    std::shared_ptr<Node> GetNodeByName(const std::string&) const;
    std::shared_ptr<Light> GetLightByName(const std::string&) const;
    std::shared_ptr<Camera> GetCameraByName(const std::string&) const;
    const std::vector<std::shared_ptr<Node>>& RootNodes();
    const std::vector<std::shared_ptr<Node>>& Nodes();
    const std::vector<std::shared_ptr<Light>> Lights() const;
    const std::vector<std::shared_ptr<Camera>> Cameras() const;
    const std::vector<std::shared_ptr<Animation>> Animations() const;
    glm::vec3 Up() const;
    void SetUp(glm::vec3);

protected:
    Scene(const std::string& name);

private:
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float delta);
    virtual void _UpdateGPU(float delta);
    virtual void _FixedUpdateCPU(float delta);
    virtual void _FixedUpdateGPU(float delta);
    void _AddNodeChildren(std::shared_ptr<Node> node);
    glm::vec3 _up { Common::Up() };
    std::string _name;
    std::vector<std::shared_ptr<Node>> _rootNodes;
    std::vector<std::shared_ptr<Node>> _nodes;
    std::shared_ptr<Camera> _currentCamera { nullptr };
    std::shared_ptr<AABB> _aabb { nullptr };
    PhysicsEngine _physicsEngine;
};