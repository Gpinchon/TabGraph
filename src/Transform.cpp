#include "Transform.hpp"

std::shared_ptr<Transform> Transform::Create(const std::string &name)
{
    return std::shared_ptr<Transform>(new Transform(name));
}

std::shared_ptr<Transform> Transform::shared_from_this()
{
    return std::static_pointer_cast<Transform>(Object::shared_from_this());
}

glm::mat4 Transform::WorldTransformMatrix()
{
    return Parent() ? Parent()->WorldTransformMatrix() * LocalTransformMatrix() : LocalTransformMatrix();
}

glm::mat4 Transform::WorldTranslationMatrix() const
{
    return Parent() ? Parent()->WorldTransformMatrix() * LocalTranslationMatrix() : LocalTranslationMatrix();
}

glm::mat4 Transform::WorldRotationMatrix() const
{
    return Parent() ? Parent()->WorldTransformMatrix() * LocalRotationMatrix() : LocalRotationMatrix();
}

glm::mat4 Transform::WorldScaleMatrix() const
{
    return Parent() ? Parent()->WorldTransformMatrix() * LocalScaleMatrix() : LocalScaleMatrix();
}

glm::mat4 Transform::LocalTransformMatrix()
{
    if (_needsUpdate) {
        _localTransformMatrix = LocalTranslationMatrix() * LocalRotationMatrix() * LocalScaleMatrix();
        _needsUpdate = false;
    }
    return _localTransformMatrix;
}

glm::mat4 Transform::LocalTranslationMatrix() const
{
    return glm::translate(Position());
}

glm::mat4 Transform::LocalRotationMatrix() const
{
    return glm::mat4_cast(Rotation());
}

glm::mat4 Transform::LocalScaleMatrix() const
{
    return glm::scale(Scale());
}

void Transform::SetLocalTransform(glm::mat4 transform) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    SetPosition(translation);
    SetRotation(rotation);
    SetScale(scale);
}

glm::vec3 Transform::WorldPosition() const
{
    return Parent() ? Parent()->WorldTransformMatrix() * glm::vec4(Position(), 1.f) : Position();
}

glm::quat Transform::WorldRotation() const
{
    return Parent() ? Parent()->WorldTransformMatrix() * glm::mat4_cast(Rotation()) : Rotation();
}

glm::vec3 Transform::WorldScale() const
{
    return Parent() ? Parent()->WorldTransformMatrix() * glm::vec4(Scale(), 1.f) : Scale();
}

/** @return the node local position */
glm::vec3 Transform::Position() const
{
    return _position;
}

/** @argument position : the node local position */
void Transform::SetPosition(glm::vec3 position)
{
    _needsUpdate |= _position != position;
    _position = position;
}

/** @return the node local rotation */
glm::quat Transform::Rotation() const
{
    return _rotation;
}

/** @argument rotation : the node local rotation */
void Transform::SetRotation(glm::vec3 rotation)
{
    auto quatRotation = glm::quat(rotation);
    _needsUpdate |= _rotation != quatRotation;
    _rotation = quatRotation;
}

/** @return the node local scale */
void Transform::SetRotation(glm::quat rotation)
{
    _needsUpdate |= _rotation != rotation;
    _rotation = rotation;
}

/** @return the node local scale */
glm::vec3 Transform::Scale() const
{
    return _scale;
}

/** @argument scale : the node local scale */
void Transform::SetScale(glm::vec3 scale)
{
    _needsUpdate |= _scale != scale;
    _scale = scale;
}

/**
 * @brief Common::Forward() * Rotation()
 * READONLY : Computed on demand
 */
glm::vec3 Transform::Forward() const
{
    return Rotation() * Common::Forward();
}

/**
 * @brief READONLY : Computed on demand
 * @return Common::Up() * Rotation()
 */
glm::vec3 Transform::Up() const
{
    return Rotation() * Common::Up();
}

/**
 * @brief READONLY : Computed on demand
 * Common::Right() * Rotation()
 */
glm::vec3 Transform::Right() const
{
    return Rotation() * Common::Right();
}

void Transform::LookAt(const glm::vec3 &target, const glm::vec3 &up)
{
    auto direction(normalize(target - WorldPosition()));
    SetRotation(glm::quatLookAt(direction, up));
}

void Transform::LookAt(const std::shared_ptr<Transform> &target, const glm::vec3 &up)
{
    if (target == nullptr)
        return;
    LookAt(target->WorldPosition(), up);
}

std::shared_ptr<Transform> Transform::Parent() const
{
    return _parent;
}

void Transform::SetParent(std::shared_ptr<Transform> parent)
{
    if (parent == shared_from_this() || Parent() == parent)
        return;
    _parent = parent;
}