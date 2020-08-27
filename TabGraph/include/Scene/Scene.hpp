/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 23:59:35
*/
#pragma once

#include "Common.hpp"
#include "Component.hpp"

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
class Scene : public Component {
public:
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
    virtual void Render(const RenderMod&);
    virtual void RenderDepth(const RenderMod&);
    std::shared_ptr<AABB> GetLimits() const;
    std::shared_ptr<Node> GetNode(std::shared_ptr<Node>) const;
    std::shared_ptr<Node> GetNodeByName(const std::string&) const;
    std::shared_ptr<Light> GetLightByName(const std::string&) const;
    std::shared_ptr<Camera> GetCameraByName(const std::string&) const;
    const std::vector<std::shared_ptr<Node>> RootNodes() const;
    const std::vector<std::shared_ptr<Node>> Nodes() const;
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
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float delta) override;
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
    glm::vec3 _up { Common::Up() };
    std::string _name;
    std::shared_ptr<Camera> _currentCamera { nullptr };
    std::shared_ptr<AABB> _aabb { nullptr };
};