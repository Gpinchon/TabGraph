#pragma once
#include "Common.hpp"
#include "Object.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

/** Header-only class for performance */
class Transform : public Object
{
public:
    static std::shared_ptr<Transform> Create(const std::string &name);
    std::shared_ptr<Transform> shared_from_this();
    glm::mat4 WorldTransformMatrix();
    glm::mat4 WorldTranslationMatrix() const;
    glm::mat4 WorldRotationMatrix() const;
    glm::mat4 WorldScaleMatrix() const;
    glm::mat4 LocalTransformMatrix();
    glm::mat4 LocalTranslationMatrix() const;
    glm::mat4 LocalRotationMatrix() const;
    glm::mat4 LocalScaleMatrix() const;
    void SetLocalTransform(glm::mat4 transform);
    glm::vec3 WorldPosition() const;
    glm::quat WorldRotation() const;
    glm::vec3 WorldScale() const;
	/** @return the node local position */
    glm::vec3 Position() const;
    /** @argument position : the node local position */
    void SetPosition(glm::vec3 position);
    /** @return the node local rotation */
    glm::quat Rotation() const;
    /** @argument rotation : the node local rotation */
    void SetRotation(glm::vec3 rotation);
    /** @return the node local scale */
    void SetRotation(glm::quat rotation);
    /** @return the node local scale */
    glm::vec3 Scale() const;
    /** @argument scale : the node local scale */
    void SetScale(glm::vec3 scale);
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
    void LookAt(const glm::vec3 &target, const glm::vec3 &up = Common::Up());
    void LookAt(const std::shared_ptr<Transform> &target, const glm::vec3 &up = Common::Up());
    std::shared_ptr<Transform> Parent() const;
    void SetParent(std::shared_ptr<Transform> parent);
    ~Transform() = default;

private:
    Transform(const std::string &name) : Object(name) {};
    /*
    void RemoveChild(std::shared_ptr<Transform> child) { _children.erase(std::remove(_children.begin(), _children.end(), child), _children.end()); };
    void AddChild(std::shared_ptr<Transform> child) {
        if (child == shared_from_this() || std::find(_children.begin(), _children.end(), child) != _children.end())
            return;
        _children.push_back(child);
        child->SetParent(shared_from_this());
    };
    */
    std::shared_ptr<Transform> _parent;
    //std::vector<std::shared_ptr<Transform>> _children;
	glm::vec3 _position { 0, 0, 0 };
    glm::quat _rotation {glm::vec3(0.0, 0.0, 0.0)};
    glm::vec3 _scale { 1, 1, 1 };
    bool _needsUpdate { true };
    glm::mat4 _localTransformMatrix { glm::mat4(1) };
};
