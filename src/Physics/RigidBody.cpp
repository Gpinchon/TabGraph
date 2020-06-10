#include "Physics/RigidBody.hpp"
#include "Node.hpp"
#include "Physics/BoundingElement.hpp"
#include "Physics/BoundingSphere.hpp"
#include <glm/gtc/quaternion.hpp>

RigidBody::RigidBody(const std::string& name)
    : Component(name)
    , _collider(new BoundingSphere(glm::vec3(0), 1.f))
{
}

std::shared_ptr<RigidBody> RigidBody::Create(const std::string& name, const std::shared_ptr<Node>& node, const std::shared_ptr<BoundingElement>& collider)
{
    auto t = std::shared_ptr<RigidBody>(new RigidBody(name));
    t->SetNode(node);
    t->SetCollider(collider);
    return t;
}
/*
const std::vector<Ray> RigidBody::GatherRays()
{
    auto direction(normalize(LinearVelocity()));
    auto localPosition(glm::vec3(0) + direction * 0.5f);
    auto worldPosition(GetNode()->TranslationMatrix() * GetNode()->ScaleMatrix() *  glm::vec4(localPosition, 1.f));

    _rays.at(0).direction = direction;
    _rays.at(0).origin = worldPosition;
    return _rays;
}
*/

bool RigidBody::Static() const
{
    return _static;
}

void RigidBody::SetStatic(bool isStatic)
{
    _static = isStatic;
}

std::shared_ptr<Node> RigidBody::GetNode() const
{
    return _node.lock();
}

void RigidBody::SetNode(std::shared_ptr<Node> target)
{
    _node = target;
}

float RigidBody::Mass() const
{
    return _mass;
}

void RigidBody::SetMass(const float mass)
{
    _mass = mass;
    _invMass = 1 / mass;
    SetMomentOfInertia(glm::vec3(2.f / 5.f * _mass * 0.25));
}

glm::quat RigidBody::AngularSpin() const
{
    /*glm::quat q(0,
                AngularVelocity().x,
                AngularVelocity().y,
                AngularVelocity().z);
    return q;*/
    return AngularVelocity();
}

glm::vec3 RigidBody::LinearAcceleration() const
{
    return _totalForce;
}

glm::vec3 RigidBody::AngularFactor() const
{
    return _angularFactor;
}

void RigidBody::SetAngularFactor(glm::vec3 angularFactor)
{
    _angularFactor = angularFactor;
}

glm::vec3 RigidBody::LinearFactor() const
{
    return _linearFactor;
}

void RigidBody::SetLinearFactor(glm::vec3 linearFactor)
{
    _linearFactor = linearFactor;
}

glm::vec3 RigidBody::MomenfOfIndertia() const
{
    return _momentOfInertia;
}

void RigidBody::SetMomentOfInertia(glm::vec3 momentOfInertia)
{
    _momentOfInertia = momentOfInertia;
    _invMomentOfInertia = 1.f / momentOfInertia;
}

glm::vec3 RigidBody::LinearVelocity() const
{
    return _linearVelocity;
}

void RigidBody::SetLinearVelocity(const glm::vec3 velocity)
{
    _linearVelocity = velocity;
}

glm::vec3 RigidBody::AngularVelocity() const
{
    return _angularVelocity;
}

void RigidBody::SetAngularVelocity(const glm::vec3 velocity)
{
    _angularVelocity = velocity;
}

glm::vec3 RigidBody::GravityForce() const
{
    return _gravity;
}

void RigidBody::SetGravityForce(const glm::vec3 gravity)
{
    _gravity = gravity;
}

bool RigidBody::ApplyGravity() const
{
    return _applyGravity;
}

void RigidBody::SetApplyGravity(const bool applyGravity)
{
    _applyGravity = applyGravity;
}

void RigidBody::ApplyTorque(glm::vec3 torque)
{
    SetTotalTorque(TotalTorque() + torque * AngularFactor());
}

void RigidBody::ApplyLocalForce(glm::vec3 force, glm::vec3 forceLocation)
{
    ApplyCentralForce(force);
    ApplyTorque(glm::cross(forceLocation, force * AngularFactor()));
}

void RigidBody::ApplyCentralForce(glm::vec3 force)
{
    SetTotalForce(TotalForce() + force * Mass());
}

void RigidBody::ApplyCentralPush(glm::vec3 pushDirection)
{
    SetLinearVelocity(LinearVelocity() + pushDirection / Mass());
}

void RigidBody::ApplyLocalPush(glm::vec3 pushDirection, glm::vec3 pushLocation)
{
    ApplyCentralPush(pushDirection);
    SetAngularVelocity(AngularVelocity() + glm::cross(pushLocation, pushDirection));
}

void RigidBody::ApplyWorldPush(glm::vec3 pushDirection, glm::vec3 pushLocation, glm::vec3 originalPosition)
{
    ApplyLocalPush(pushDirection, pushLocation - originalPosition);
}

glm::vec3 RigidBody::TotalForce() const
{
    return _totalForce;
}

void RigidBody::SetTotalForce(glm::vec3 totalForce)
{
    _totalForce = totalForce;
}

glm::vec3 RigidBody::TotalTorque() const
{
    return _totalTorque;
}

void RigidBody::SetTotalTorque(glm::vec3 totalTorque)
{
    _totalTorque = totalTorque;
}

void RigidBody::SetCollider(const std::shared_ptr<BoundingElement>& collider)
{
    _collider = collider;
}

std::shared_ptr<BoundingElement>& RigidBody::GetCollider()
{
    return _collider;
}

#include "Tools.hpp"

#define HALF_PI (M_PI * 0.5f)

void RigidBody::IntegrateVelocities(float step)
{
    if (Static())
        return;
    if (ApplyGravity())
        SetLinearVelocity(LinearVelocity() + (TotalForce() + GravityForce()) * (_invMass * step));
    else
        SetLinearVelocity(LinearVelocity() + TotalForce() * (_invMass * step));
    SetAngularVelocity(AngularVelocity() + TotalTorque() * (_invMomentOfInertia * step));
    //m_linearVelocity += m_totalForce * (m_inverseMass * step);
    //m_angularVelocity += m_invInertiaTensorWorld * m_totalTorque * step;

#define MAX_ANGVEL HALF_PI
    /// clamp angular velocity. collision calculations will fail on higher angular velocities
    float angvel = AngularVelocity().length();
    if (angvel * step > MAX_ANGVEL) {
        SetAngularVelocity(AngularVelocity() * float(MAX_ANGVEL / step) / angvel);
    }
}
/*

bool        intersect_test(float t0, float t1)
{
    return (t0 > 0.f || t1 > 0.f);
}

bool        solve_quadratic(float a, float b, float c, float *t)
{
    float   discrim;
    float   q;

    discrim = (b * b - 4.f * a * c);
    if (discrim < 0)
        return false;
    q = -.5f * (b < 0 ? (b - sqrtf(discrim)) : (b + sqrtf(discrim)));
    auto t0 = q / a;
    auto t1 = c / q;
    if (t0 > t1)
    {
        q = t0;
        t0 = t1;
        t1 = q;
    }
    *t = glm::min(t0, t1);
    return intersect_test(t0, t1);
}

glm::vec3    sphere_normal(glm::vec3 position, glm::vec3 intersectPosition, float radius)
{
    return normalize((position - intersectPosition) / radius);
}

Intersection SphereIntersection(const Ray ray, glm::vec3 position, float radius, float radius2)
{
    glm::vec3       eye;
    Intersection    inter;

    eye = ray.origin - position;
    if (!(inter.intersects = solve_quadratic(
                glm::dot(ray.direction, ray.direction),
                glm::dot(eye, ray.direction) * 2.0,
                glm::dot(eye, eye) - radius2,
                &inter.distance)))
        return (inter);
    inter.position = ray.origin + ray.direction * inter.distance;
    inter.normal = sphere_normal(inter.position, inter.position, radius);
    return (inter);
}*/

/** Behaves as if it was a sphere with radius 0.5 */
/*
Intersection RigidBody::IntersectRay(const Ray ray)
{
    auto t(GetNode());

    if (t == nullptr)
        return Intersection();
    return SphereIntersection(ray, t->Position(), 0.5, 0.25);
}
*/

/*
SphereImpostor::SphereImpostor(const std::string &name) : RigidBody(name) {}

std::shared_ptr<SphereImpostor> SphereImpostor::Create(const std::string &name, std::shared_ptr<Node> node, float radius)
{
    auto t = std::shared_ptr<SphereImpostor>(new SphereImpostor(name));
    t->SetNode(node);
    t->SetRadius(radius);
    t->_rays.resize(1);
    return t;
}

Intersection SphereImpostor::IntersectRay(const Ray ray)
{
    auto t(GetNode());

    if (t == nullptr)
        return Intersection();
    return SphereIntersection(ray, t->Position(), Radius(), _radius2);
}

const std::vector<Ray> SphereImpostor::GatherRays()
{
    auto direction(normalize(LinearVelocity()));
    auto localPosition(glm::vec3(0) + direction * Radius());
    auto worldPosition(GetNode()->TranslationMatrix() * GetNode()->ScaleMatrix() *  glm::vec4(localPosition, 1.f));

    _rays.at(0).direction = direction;
    _rays.at(0).origin = worldPosition;
    return _rays;
}

float SphereImpostor::Radius() const
{
    return _radius;
}

void SphereImpostor::SetRadius(float radius)
{
    _radius = radius;
    _radius2 = radius * radius;
}
*/