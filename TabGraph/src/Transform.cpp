/*
* @Author: gpinchon
* @Date:   2020-08-27 18:48:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-11-30 22:39:45
*/
#include "Transform.hpp"

#define WORLDTRANSFORMMATRIX(transform) (transform ? transform->WorldTransformMatrix() : glm::mat4(1.f))

static size_t s_transformNbr = 0;

Transform::Transform() : Transform("Transform_" + std::to_string(++s_transformNbr))
{
}

glm::mat4 Transform::WorldTransformMatrix()
{
    if (_worldTransformNeedsUpdate) {
        _SetWorldTransformMatrix(WORLDTRANSFORMMATRIX(GetParent()) * GetLocalTransformMatrix());
        _worldTransformNeedsUpdate = false;
    }
    return _worldTransformMatrix;
}

glm::mat4 Transform::WorldTranslationMatrix()
{
    return WORLDTRANSFORMMATRIX(GetParent()) * GetLocalTranslationMatrix();
}

glm::mat4 Transform::WorldRotationMatrix()
{
    return WORLDTRANSFORMMATRIX(GetParent()) * GetLocalRotationMatrix();
}

glm::mat4 Transform::WorldScaleMatrix()
{
    return WORLDTRANSFORMMATRIX(GetParent()) * GetLocalScaleMatrix();
}

inline glm::mat4 Transform::GetLocalTransformMatrix() {
    if (_positionChanged || _rotationChanged || _scaleChanged) {
        _SetLocalTransformMatrix(GetLocalTranslationMatrix() * GetLocalRotationMatrix() * GetLocalScaleMatrix());
    }
    return _localTransformMatrix;
}

inline glm::mat4 Transform::GetLocalTranslationMatrix() {
    if (_positionChanged) {
        _SetLocalTranslationMatrix(glm::translate(GetPosition()));
        _positionChanged = false;
    }
    return _localTranslationMatrix;
}

inline glm::mat4 Transform::GetLocalRotationMatrix() {
    if (_rotationChanged) {
        _SetLocalRotationMatrix(glm::mat4_cast(GetRotation()));
        _rotationChanged = false;
    }
    return _localRotationMatrix;
}

inline glm::mat4 Transform::GetLocalScaleMatrix() {
    if (_scaleChanged) {
        _SetLocalScaleMatrix(glm::scale(GetScale()));
        _scaleChanged = false;
    }
    return _localScaleMatrix;
}

glm::vec3 Transform::WorldPosition() const
{
    return WORLDTRANSFORMMATRIX(GetParent()) * glm::vec4(GetPosition(), 1.f);
}

glm::quat Transform::WorldRotation() const
{
    return WORLDTRANSFORMMATRIX(GetParent()) * glm::mat4_cast(GetRotation());
}

glm::vec3 Transform::WorldScale() const
{
    return WORLDTRANSFORMMATRIX(GetParent()) * glm::vec4(GetScale(), 1.f);
}

/** @argument rotation : the node local rotation */
void Transform::SetRotation(glm::vec3 rotation)
{
    SetRotation(glm::quat(rotation));
}

/**
 * @brief Common::Forward() * Rotation()
 * READONLY : Computed on demand
 */
glm::vec3 Transform::Forward() const
{
    return GetRotation() * Common::Forward();
}

/**
 * @brief READONLY : Computed on demand
 * @return Common::Up() * Rotation()
 */
glm::vec3 Transform::Up() const
{
    return GetRotation() * Common::Up();
}

/**
 * @brief READONLY : Computed on demand
 * Common::Right() * Rotation()
 */
glm::vec3 Transform::Right() const
{
    return GetRotation() * Common::Right();
}

void Transform::LookAt(const glm::vec3& target, const glm::vec3& up)
{
    auto direction(normalize(target - WorldPosition()));
    float directionLength = glm::length(direction);

    // Check if the direction is valid; Also deals with NaN
    if (!(directionLength > 0.0001))
        SetRotation(glm::quat(1, 0, 0, 0)); // Just return identity

    // Normalize direction
    direction /= directionLength;

    // Is the normal up (nearly) parallel to direction?
    if (glm::abs(glm::dot(direction, up)) > .9999f) {
        // Use alternative up
        SetRotation(glm::quatLookAt(direction, glm::vec3(1, 0, 0)));
    } else {
        SetRotation(glm::quatLookAt(direction, up));
    }
    //SetRotation(glm::quatLookAt(direction, up));
}

void Transform::LookAt(const std::shared_ptr<Transform>& target, const glm::vec3& up)
{
    if (target == nullptr)
        return;
    LookAt(target->WorldPosition(), up);
}

std::shared_ptr<Transform> Transform::GetParent() const
{
    return _parent.lock();
}

void Transform::SetParent(std::shared_ptr<Transform> parent)
{
    if (parent == shared_from_this() || GetParent() == parent)
        return;
    _parent = parent;
    parent->WorldPositionChanged.ConnectMember<Transform>(std::static_pointer_cast<Transform>(shared_from_this()), &Transform::_OnParentPositionChanged);
    parent->WorldRotationChanged.ConnectMember<Transform>(std::static_pointer_cast<Transform>(shared_from_this()), &Transform::_OnParentRotationChanged);
    parent->WorldScaleChanged.ConnectMember<Transform>(std::static_pointer_cast<Transform>(shared_from_this()), &Transform::_OnParentScaleChanged);
    //parent->WorldTransformMatrixChanged.ConnectMember<Transform>(std::static_pointer_cast<Transform>(shared_from_this()), &Transform::_OnTransformUpdate);
    ParentChanged.Emit(parent);
}

inline void Transform::_FixedUpdateCPU(float) {
    /*if (_worldTransformNeedsUpdate)
    {
        _SetWorldTransformMatrix(GetParent() ? GetParent()->WorldTransformMatrix() : glm::mat4(1.f) * GetLocalTransformMatrix());
        _worldTransformNeedsUpdate = false;
    }*/
}

void Transform::_SetWorldTransformMatrix(const glm::mat4& matrix)
{
    if (matrix != _worldTransformMatrix) {
        _worldTransformMatrix = matrix;
        WorldTransformMatrixChanged.Emit(matrix);
    }
}

inline void Transform::_SetLocalTransformMatrix(const glm::mat4& matrix) {
    if (matrix != _localTransformMatrix) {
        _localTransformMatrix = matrix;
        LocalTransformMatrixChanged.Emit(matrix);
    }
}

inline void Transform::_SetLocalTranslationMatrix(const glm::mat4& matrix) {
    if (matrix != _localTranslationMatrix) {
        _localTranslationMatrix = matrix;
        LocalTranslationMatrixChanged.Emit(matrix);
    }
}

inline void Transform::_SetLocalRotationMatrix(const glm::mat4& matrix) {
    if (matrix != _localRotationMatrix) {
        _localRotationMatrix = matrix;
        LocalRotationMatrixChanged.Emit(matrix);
    }
}

inline void Transform::_SetLocalScaleMatrix(const glm::mat4& matrix) {
    if (matrix != _localScaleMatrix) {
        _localScaleMatrix = matrix;
        LocalScaleMatrixChanged.Emit(matrix);
    }
}
