/*
* @Author: gpinchon
* @Date:   2020-08-27 18:48:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-12-23 18:47:58
*/
#include "Transform.hpp"

#define WORLDTRANSFORMMATRIX(transform) (transform ? transform->WorldTransformMatrix() : glm::mat4(1.f))

static size_t s_transformNbr = 0;

Transform::Transform()
    : Component("Transform_" + std::to_string(++s_transformNbr))
{
    PositionChanged.ConnectMember(this, &Transform::_OnPositionChanged);
    RotationChanged.ConnectMember(this, &Transform::_OnRotationChanged);
    ScaleChanged.ConnectMember(this, &Transform::_OnScaleChanged);
}

Transform::Transform(const Transform& transform)
    : Component(transform)
    , _Position(transform._Position)
    , _Rotation(transform._Rotation)
    , _Scale(transform._Scale)
    , _positionChanged(transform._positionChanged)
    , _rotationChanged(transform._rotationChanged)
    , _scaleChanged(transform._scaleChanged)
    , _localTransformMatrix(transform._localTransformMatrix)
    , _localTranslationMatrix(transform._localTranslationMatrix)
    , _localRotationMatrix(transform._localRotationMatrix)
    , _localScaleMatrix(transform._localScaleMatrix)
    , _parent(transform._parent)
{
    PositionChanged.ConnectMember(this, &Transform::_OnPositionChanged);
    RotationChanged.ConnectMember(this, &Transform::_OnRotationChanged);
    ScaleChanged.ConnectMember(this, &Transform::_OnScaleChanged);
}

Transform::Transform(const std::string& name)
    : Transform()
{
    SetName(name);
};

glm::mat4 Transform::WorldTransformMatrix()
{
    return WORLDTRANSFORMMATRIX(GetParent()) * GetLocalTransformMatrix();
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

inline glm::mat4 Transform::GetLocalTransformMatrix()
{
    if (_positionChanged || _rotationChanged || _scaleChanged) {
        _SetLocalTransformMatrix(GetLocalTranslationMatrix() * GetLocalRotationMatrix() * GetLocalScaleMatrix());
    }
    return _localTransformMatrix;
}

inline glm::mat4 Transform::GetLocalTranslationMatrix()
{
    if (_positionChanged) {
        _SetLocalTranslationMatrix(glm::translate(GetPosition()));
        _positionChanged = false;
    }
    return _localTranslationMatrix;
}

inline glm::mat4 Transform::GetLocalRotationMatrix()
{
    if (_rotationChanged) {
        _SetLocalRotationMatrix(glm::mat4_cast(GetRotation()));
        _rotationChanged = false;
    }
    return _localRotationMatrix;
}

inline glm::mat4 Transform::GetLocalScaleMatrix()
{
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
    if (GetParent() == parent || parent.get() == this)
        return;
    _parent = parent;
    ParentChanged.Emit(parent);
}

inline void Transform::_FixedUpdateCPU(float)
{
}

inline void Transform::_SetLocalTransformMatrix(const glm::mat4& matrix)
{
    if (matrix != _localTransformMatrix) {
        _localTransformMatrix = matrix;
        LocalTransformMatrixChanged.Emit(matrix);
    }
}

inline void Transform::_SetLocalTranslationMatrix(const glm::mat4& matrix)
{
    if (matrix != _localTranslationMatrix) {
        _localTranslationMatrix = matrix;
        LocalTranslationMatrixChanged.Emit(matrix);
    }
}

inline void Transform::_SetLocalRotationMatrix(const glm::mat4& matrix)
{
    if (matrix != _localRotationMatrix) {
        _localRotationMatrix = matrix;
        LocalRotationMatrixChanged.Emit(matrix);
    }
}

inline void Transform::_SetLocalScaleMatrix(const glm::mat4& matrix)
{
    if (matrix != _localScaleMatrix) {
        _localScaleMatrix = matrix;
        LocalScaleMatrixChanged.Emit(matrix);
    }
}
