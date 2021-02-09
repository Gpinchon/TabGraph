#include "Physics/RigidBody.hpp"
#include "Node.hpp"
#include "Physics/BoundingElement.hpp"
#include "Physics/BoundingSphere.hpp"
#include <glm/gtc/quaternion.hpp>

RigidBody::RigidBody(const std::string& name, const std::shared_ptr<Node>& node, const std::shared_ptr<BoundingElement>& collider)
    : Component(name)
{
    SetNode(node);
    SetCollider(collider);
}

std::shared_ptr<Node> RigidBody::GetNode() const
{
    return GetComponent<Node>();
}

void RigidBody::SetNode(std::shared_ptr<Node> target)
{
    SetComponent(target);
}

glm::mat4 RigidBody::GetCurrentTransform() const
{
    return glm::translate(GetCurrentPosition()) * glm::mat4(GetCurrentRotation()) * glm::scale(GetCurrentScale());
}

glm::mat4 RigidBody::GetNextTransform() const
{
    return glm::translate(GetNextPosition()) * glm::mat4(GetNextRotation()) * glm::scale(GetNextScale());
}

void RigidBody::ApplyAngularImpulse(const glm::vec3& impulse)
{
    SetAngularVelocity(GetAngularVelocity() + InvInertiaTensor() * impulse);
}

void RigidBody::ApplyLinearImpulse(const glm::vec3& impulse)
{
    SetLinearVelocity(GetLinearVelocity() + GetInvMass() * impulse);
}

void RigidBody::SetMass(const float mass)
{
    _SetMass(mass);
    _SetInvMass(mass ? (1.f / mass) : 0.f);
    SetInertiaLocal(glm::vec3((2.f / 5.f) * mass * 0.25));
}

glm::quat RigidBody::AngularSpin() const
{
    return GetAngularVelocity();
}

void RigidBody::SetInertiaLocal(glm::vec3 inertia)
{
    _SetInertiaLocal(inertia);
    _SetInvInertiaLocal(glm::vec3(
        inertia.x ? 1.f / inertia.x : 0.f,
        inertia.y ? 1.f / inertia.y : 0.f,
        inertia.z ? 1.f / inertia.z : 0.f));
}

glm::mat3 RigidBody::InertiaTensor() const
{
    auto &transform = GetCurrentTransform();
    auto localInertiaTensor = GetCollider()->LocalInertiaTensor(GetMass());
    glm::mat3 inverseLocalToWorld = glm::inverse(transform);
    return glm::transpose(inverseLocalToWorld) * localInertiaTensor * inverseLocalToWorld;
}

glm::mat3 RigidBody::InvInertiaTensor() const
{
    return GetMass() ? glm::inverse(InertiaTensor()) : glm::mat3(0.f);
}

void RigidBody::SetLinearVelocity(const glm::vec3 velocity)
{
    if (GetStatic())
        return;
    _SetLinearVelocity(velocity);
}

void RigidBody::SetAngularVelocity(const glm::vec3 velocity)
{
    if (GetStatic())
        return;
    _SetAngularVelocity(velocity);
}

void RigidBody::ApplyTorque(glm::vec3 torque)
{
    SetTotalTorque(GetTotalTorque() + torque * GetAngularFactor());
}

void RigidBody::ApplyLocalForce(glm::vec3 force, glm::vec3 forceLocation)
{
    ApplyCentralForce(force);
    ApplyTorque(glm::cross(forceLocation, force * GetAngularFactor()));
}

void RigidBody::ApplyCentralForce(glm::vec3 force)
{
    SetTotalForce(GetTotalForce() + force * GetMass());
}

void RigidBody::ApplyCentralPush(glm::vec3 pushDirection)
{
    SetLinearVelocity(GetLinearVelocity() + pushDirection * GetInvMass());
}

void RigidBody::ApplyLocalPush(glm::vec3 pushDirection, glm::vec3 pushLocation)
{
    ApplyCentralPush(pushDirection);
    SetAngularVelocity(GetAngularVelocity() + InvInertiaTensor() * glm::cross(pushLocation, pushDirection));
}

void RigidBody::ApplyWorldPush(glm::vec3 pushDirection, glm::vec3 pushLocation, glm::vec3 originalPosition)
{
    ApplyLocalPush(pushDirection, pushLocation - originalPosition);
}

void RigidBody::SetCollider(const std::shared_ptr<BoundingElement>& collider)
{
    SetComponent(collider);
}

std::shared_ptr<BoundingElement> RigidBody::GetCollider() const
{
    return GetComponent<BoundingElement>();
}

#include "Tools/Tools.hpp"

#define HALF_PI (M_PI * 0.5f)

void RigidBody::IntegrateVelocities(float step)
{
    if (GetStatic())
        return;
    if (GetApplyGravity())
        SetLinearVelocity(GetLinearVelocity() + (GetTotalForce() + GetGravityForce()) * (GetInvMass() * step));
    else
        SetLinearVelocity(GetLinearVelocity() + GetTotalForce() * (GetInvMass() * step));
    SetAngularVelocity(GetAngularVelocity() + GetTotalTorque() * (GetInvInertiaLocal() * step));
    //m_linearVelocity += m_totalForce * (m_inverseMass * step);
    //m_angularVelocity += m_invInertiaTensorWorld * m_totalTorque * step;

#define MAX_ANGVEL HALF_PI
    /// clamp angular velocity. collision calculations will fail on higher angular velocities
    float angvel = GetAngularVelocity().length();
    if (angvel * step > MAX_ANGVEL) {
        SetAngularVelocity(GetAngularVelocity() * float(MAX_ANGVEL / step) / angvel);
    }
}
