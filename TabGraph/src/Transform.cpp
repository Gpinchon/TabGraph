/*
* @Author: gpinchon
* @Date:   2020-08-27 18:48:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-11-30 22:39:45
*/
#include "Transform.hpp"

#define WORLDTRANSFORMMATRIX(transform) (transform ? transform->WorldTransformMatrix() : glm::mat4(1.f))

glm::mat4 Transform::WorldTransformMatrix() const
{
    return WORLDTRANSFORMMATRIX(Parent()) * LocalTransformMatrix();
}

glm::mat4 Transform::WorldTranslationMatrix() const
{
    return WORLDTRANSFORMMATRIX(Parent()) * LocalTranslationMatrix();
}

glm::mat4 Transform::WorldRotationMatrix() const
{
    return WORLDTRANSFORMMATRIX(Parent()) * LocalRotationMatrix();
}

glm::mat4 Transform::WorldScaleMatrix() const
{
    return WORLDTRANSFORMMATRIX(Parent()) * LocalScaleMatrix();
}

glm::mat4 Transform::LocalTransformMatrix() const
{
    return LocalTranslationMatrix() * LocalRotationMatrix() * LocalScaleMatrix();
}

glm::mat4 Transform::LocalTranslationMatrix() const
{
    return glm::translate(GetPosition());
}

glm::mat4 Transform::LocalRotationMatrix() const
{
    return glm::mat4_cast(GetRotation());
}

glm::mat4 Transform::LocalScaleMatrix() const
{
    return glm::scale(GetScale());
}

glm::vec3 Transform::WorldPosition() const
{
    return WORLDTRANSFORMMATRIX(Parent()) * glm::vec4(GetPosition(), 1.f);
}

glm::quat Transform::WorldRotation() const
{
    return WORLDTRANSFORMMATRIX(Parent()) * glm::mat4_cast(GetRotation());
}

glm::vec3 Transform::WorldScale() const
{
    return WORLDTRANSFORMMATRIX(Parent()) * glm::vec4(GetScale(), 1.f);
}

/** @return the node local position */
//glm::vec3 Transform::Position() const
//{
//    return _position;
//}

/** @argument position : the node local position */
//void Transform::SetPosition(glm::vec3 position)
//{
//    _needsUpdate |= _position != position;
//    _position = position;
//}

/** @return the node local rotation */
//glm::quat Transform::Rotation() const
//{
//    return _rotation;
//}

/** @argument rotation : the node local rotation */
void Transform::SetRotation(glm::vec3 rotation)
{
    SetRotation(glm::quat(rotation));
}

/** @return the node local scale */
//void Transform::SetRotation(glm::quat rotation)
//{
//    _needsUpdate |= _rotation != rotation;
//    _rotation = rotation;
//}

/** @return the node local scale */
//glm::vec3 Transform::Scale() const
//{
//    return _scale;
//}

/** @argument scale : the node local scale */
//void Transform::SetScale(glm::vec3 scale)
//{
//    _needsUpdate |= _scale != scale;
//    _scale = scale;
//}

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

std::shared_ptr<Transform> Transform::Parent() const
{
    return _parent.lock();
}

void Transform::SetParent(std::shared_ptr<Transform> parent)
{
    if (parent == shared_from_this() || Parent() == parent)
        return;
    //parent->PositionChanged.ConnectMember(std::static_pointer_cast<Transform>(shared_from_this()), )
    _parent = parent;
}
