/*
* @Author: gpinchon
* @Date:   2021-06-19 14:57:45
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-26 23:01:57
*/
#pragma once

#include <Common.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <set>
#include <string>

////////////////////////////////////////////////////////////////////////////////
//Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Node : public Core::Inherit<Core::Object, Node>, public std::enable_shared_from_this<Node> {
public:
    Node();
    Node(const Node& node);
    Node(const std::string& name) : Node()
    {
        SetName(name);
    }
    /** @return the Node's parent */
    auto GetParent() const
    {
        return _parent.lock();
    }
    /**
     * @brief sets the parent to parent and calls AddChild in parent
     * @param parent : the Node's new parent
    */
    void SetParent(std::shared_ptr<Node> parent)
    {
        if (GetParent() != nullptr)
            GetParent()->_RemoveChild(std::static_pointer_cast<Node>(shared_from_this()));
        _parent = parent;
        if (parent != nullptr)
            parent->_AddChild(std::static_pointer_cast<Node>(shared_from_this()));
    }

    inline auto GetLocalPosition() const
    {
        return _position;
    }
    inline auto GetLocalScale() const
    {
        return _scale;
    }
    inline auto GetLocalRotation() const
    {
        return _rotation;
    }
    inline glm::vec3 GetWorldPosition() const
    {
        return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * glm::vec4(GetLocalPosition(), 1);
    }
    inline glm::quat GetWorldRotation() const
    {
        return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * glm::mat4_cast(GetLocalRotation());
    }
    inline glm::vec3 GetWorldScale() const
    {
        return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * glm::vec4(GetLocalScale(), 1);
    }

    inline void SetLocalPosition(const glm::vec3& position)
    {
        _positionChanged |= position != GetLocalPosition();
        _position = position;
    }
    inline void SetLocalScale(const glm::vec3& scale)
    {
        _scaleChanged |= scale != GetLocalScale();
        _scale = scale;
    }
    inline void SetLocalRotation(const glm::quat& rotation)
    {
        _rotationChanged |= rotation != GetLocalRotation();
        _rotation = rotation;
    }

    glm::mat4 GetLocalTransformMatrix();
    glm::mat4 GetLocalTranslationMatrix();
    glm::mat4 GetLocalRotationMatrix();
    glm::mat4 GetLocalScaleMatrix();
    inline glm::mat4 GetWorldTransformMatrix()
    {
        return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * GetLocalTransformMatrix();
    }
    inline auto GetWorldTranslationMatrix()
    {
        return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * GetLocalTranslationMatrix();
    }
    inline auto GetWorldRotationMatrix()
    {
        return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * GetLocalRotationMatrix();
    }
    inline auto GetWorldScaleMatrix()
    {
        return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * GetLocalScaleMatrix();
    }

    /** @return true if @arg child is in children list */
    inline bool HasChild(std::shared_ptr<Node> child)
    {
        return _children.count(child) > 0;
    };
    /** @return the number of children */
    inline size_t ChildCount() const
    {
        return _children.size();
    };
    /** @return all the children */
    inline auto& GetChildren() const
    {
        return _children;
    }
    /** removes parenting for all children */
    inline void ClearChildren()
    {
        auto children { _children };
        for (auto& child : children)
            child->SetParent(nullptr);
        _children.clear();
    }

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

private:
    inline void _AddChild(std::shared_ptr<Node> child)
    {
        _children.insert(child);
    }
    /** removes parenting for specified @arg child */
    inline void _RemoveChild(std::shared_ptr<Node> child)
    {
        _children.erase(child);
    }

    bool _positionChanged { true };
    bool _rotationChanged { true };
    bool _scaleChanged { true };

    glm::vec3 _position { 0 };
    glm::vec3 _scale { 1 };
    glm::quat _rotation { glm::vec3(0.0, 0.0, 0.0) };

    glm::mat4 _localTransformMatrix { glm::mat4(1) };
    glm::mat4 _localTranslationMatrix {};
    glm::mat4 _localRotationMatrix {};
    glm::mat4 _localScaleMatrix {};
    std::weak_ptr<Node> _parent;
    std::set<std::shared_ptr<Node>> _children;
};
};
