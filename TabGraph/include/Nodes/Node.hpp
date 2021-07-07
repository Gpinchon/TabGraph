/*
* @Author: gpinchon
* @Date:   2021-06-19 14:57:45
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-04 16:38:23
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
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
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Group;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
/**
 * @brief Describes a leaf of the SceneGraph, can only have parent
*/
class Node : public Core::Inherit<Core::Object, Node>, public std::enable_shared_from_this<Node> {
public:
    Node();
    Node(const Node& node);
    Node(const std::string& name)
        : Node()
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
    void SetParent(std::shared_ptr<Group> parent);

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

    glm::vec3 GetWorldPosition() const;
    glm::quat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;

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

    glm::mat4 GetWorldTransformMatrix();
    glm::mat4 GetWorldTranslationMatrix();
    glm::mat4 GetWorldRotationMatrix();
    glm::mat4 GetWorldScaleMatrix();

    /**
     * @brief READONLY : Computed on demand
     * @return Common::Forward() * Rotation()
     */
    glm::vec3 Forward() const {
        return Common::Forward() * GetWorldRotation();
    }
    /**
     * @brief READONLY : Computed on demand
     * @return Common::Up() * Rotation()
     */
    glm::vec3 Up() const {
        return Common::Up() * GetWorldRotation();
    }
    /**
     * @brief READONLY : Computed on demand
     * Common::Right() * Rotation()
     */
    glm::vec3 Right() const {
        return Common::Right() * GetWorldRotation();
    }

    void LookAt(const glm::vec3& target, const glm::vec3& up = Common::Up());
    void LookAt(const std::shared_ptr<Node>& target, const glm::vec3& up = Common::Up());

private:
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
    std::weak_ptr<Group> _parent;
};
}
