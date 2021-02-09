/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:42
*/

#pragma once

#include "Common.hpp"
#include "Component.hpp"
#include "Event/Signal.hpp"


#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

enum class RenderMod {
    RenderAll,
    RenderOpaque,
    RenderTransparent
};

enum class RenderPass {
    Geometry,
    Material
};

class BoundingAABB;
class RigidBody;
class Animation;
class Mesh;
class Scene;

class Node : public Component {
    /** The local position */
    PROPERTY(glm::vec3, Position, 0);
    /** The local rotation */
    PROPERTY(glm::quat, Rotation, glm::vec3(0.0, 0.0, 0.0));
    /** The local scale */
    PROPERTY(glm::vec3, Scale, 1);
public:
    Node();
    Node(const Node& node);
    Node(const std::string& name);
    virtual bool Draw(std::shared_ptr<Scene> scene, RenderPass pass, RenderMod = RenderMod::RenderAll, bool drawChildren = false);
    //virtual bool Draw(RenderPass pass, RenderMod = RenderMod::RenderAll, bool drawChildren = false);
    virtual bool DrawDepth(std::shared_ptr<Scene> scene, RenderMod = RenderMod::RenderAll, bool drawChildren = false);

    /** @return the Node's parent */
    //std::shared_ptr<Node> Parent() const { return _parent.lock(); };
    /** sets the parent to parent and calls AddChild in parent */
    //void SetParent(std::shared_ptr<Node> parent);
    void AddChild(std::shared_ptr<Node>);
    /** @return true if @arg child is in children list */
    bool HasChild(std::shared_ptr<Node> child) { return HasComponent(child); };
    /** @return the number of children */
    size_t ChildCount() const { return GetComponents<Node>().size(); };
    /** @return all the children */
    auto GetChildren() const { return GetComponents<Node>(); }
    /** removes parenting for specified @arg child */
    void RemoveChild(std::shared_ptr<Node> child);
    /** removes parenting for all children */
    void EmptyChildren();

    std::shared_ptr<BoundingAABB> GetBounds() const;

    virtual ~Node() /*= default*/;

    glm::mat4 WorldTransformMatrix();
    glm::mat4 WorldTranslationMatrix();
    glm::mat4 WorldRotationMatrix();
    glm::mat4 WorldScaleMatrix();

    Signal<glm::mat4> LocalTransformMatrixChanged;
    Signal<glm::mat4> LocalTranslationMatrixChanged;
    Signal<glm::mat4> LocalRotationMatrixChanged;
    Signal<glm::mat4> LocalScaleMatrixChanged;
    glm::mat4 GetLocalTransformMatrix();
    glm::mat4 GetLocalTranslationMatrix();
    glm::mat4 GetLocalRotationMatrix();
    glm::mat4 GetLocalScaleMatrix();

    //Signal<> WorldTransformChanged;
    glm::vec3 WorldPosition() const;
    glm::quat WorldRotation() const;
    glm::vec3 WorldScale() const;
    /** @argument rotation : the node local rotation */
    void SetRotation(glm::vec3 rotation);
    /**
     * @brief Common::Forward() * Rotation()
     * READONLY : Computed on demand
     */
    glm::vec3 Forward() const;
    /**
     * @brief READONLY : Computed on demand
     * @return Common::Up() * Rotation()
     */
    glm::vec3 Up() const;
    /**
     * @brief READONLY : Computed on demand
     * Common::Right() * Rotation()
     */
    glm::vec3 Right() const;
    void LookAt(const glm::vec3& target, const glm::vec3& up = Common::Up());
    void LookAt(const std::shared_ptr<Node>& target, const glm::vec3& up = Common::Up());
    Signal<std::shared_ptr<Node>> ParentChanged;
    std::shared_ptr<Node> GetParent() const;
    void SetParent(std::shared_ptr<Node> parent);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto newNode{ Component::Create<Node>(*this) };
        newNode->RemoveComponents<Node>(); //Remove children
        return newNode;
    }

    virtual void _UpdateMeshSkin(float);
    std::shared_ptr<BoundingAABB> _bounds { nullptr };
    float _meshSkinUpdateDelta{ 0 };
    virtual void _Replace(const std::shared_ptr<Component>& oldComponent, const std::shared_ptr<Component>& newComponent) override {
        if (GetParent() == oldComponent)
            SetParent(std::static_pointer_cast<Node>(newComponent));
        else if (HasChild(std::static_pointer_cast<Node>(newComponent)))
            std::static_pointer_cast<Node>(newComponent)->SetParent(std::static_pointer_cast<Node>(shared_from_this()));
    };
    void _SetLocalTransformMatrix(const glm::mat4& matrix);
    void _SetLocalTranslationMatrix(const glm::mat4& matrix);
    void _SetLocalRotationMatrix(const glm::mat4& matrix);
    void _SetLocalScaleMatrix(const glm::mat4& matrix);
    void _OnPositionChanged(glm::vec3 position) {
        _positionChanged = true;
    }
    void _OnRotationChanged(glm::quat rotation) {
        _rotationChanged = true;
    }
    void _OnScaleChanged(glm::vec3 scale) {
        _scaleChanged = true;
    }
    bool _positionChanged{ true };
    bool _rotationChanged{ true };
    bool _scaleChanged{ true };

    glm::mat4 _localTransformMatrix{ glm::mat4(1) };
    glm::mat4 _localTranslationMatrix{ glm::translate(glm::vec3(0)) };
    glm::mat4 _localRotationMatrix{ glm::rotate(0.f, glm::vec3(0)) };
    glm::mat4 _localScaleMatrix{ glm::scale(glm::vec3(1)) };
    std::weak_ptr<Node> _parent;
};
