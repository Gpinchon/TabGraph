#pragma once

#include "BoundingElement.hpp"
#include "BoundingSphere.hpp"
#include "Common.hpp"
#include "Component.hpp"
#include "Tools/Tools.hpp"
#include <glm/ext.hpp>
#include <glm/glm.hpp>

class Node;

/*
** @brief : A RigidBody class for PhysicsEngine
** The Transform Component is the transform that's to be applied after physics calculations 
*/
class RigidBody : public Component {
    PROPERTY(bool, Static, false);
    PROPERTY(bool, ApplyGravity, true);
    PROPERTY(float, Restitution, 1);
    PROPERTY(glm::vec3, TotalTorque, 0);
    PROPERTY(glm::vec3, TotalForce, 0);
    PROPERTY(glm::vec3, GravityForce, Common::Gravity());
    PROPERTY(glm::vec3, AngularFactor, 1);
    READONLYPROPERTY(glm::vec3, AngularVelocity, 0);
    PROPERTY(glm::vec3, LinearFactor, 1);
    READONLYPROPERTY(glm::vec3, LinearVelocity, 0);
    
    READONLYPROPERTY(glm::vec3, InertiaLocal, 1);
    READONLYPROPERTY(glm::vec3, InvInertiaLocal, 1)
    READONLYPROPERTY(float, Mass, 1);
    READONLYPROPERTY(float, InvMass, 1);

    PROPERTY(glm::vec3, CurrentPosition, 0);
    PROPERTY(glm::vec3, CurrentScale, 1);
    PROPERTY(glm::quat, CurrentRotation, glm::vec3(1));

    PROPERTY(glm::vec3, NextPosition, 0);
    PROPERTY(glm::vec3, NextScale, 1);
    PROPERTY(glm::quat, NextRotation, glm::vec3(1));
public:
    RigidBody(const std::string& name, const std::shared_ptr<Node>& node, const std::shared_ptr<BoundingElement>& collider);
    template <typename T, typename U,
        typename = IsSharedPointerOfType<RigidBody, T>,
        typename = IsSharedPointerOfType<RigidBody, U>>
    static bool Collides(const T& a, const U& b);

    glm::mat4 GetCurrentTransform() const;
    glm::mat4 GetNextTransform() const;

    /** @arg impulse : the impulse in world space */
    void ApplyAngularImpulse(const glm::vec3& impulse);
    /** @arg impulse : the impulse in world space */
    void ApplyLinearImpulse(const glm::vec3& impulse);

    std::shared_ptr<Node> GetNode() const;
    void SetNode(std::shared_ptr<Node> node);

    void SetMass(const float mass);

    void RigidBody::SetLinearVelocity(const glm::vec3 velocity);
    void RigidBody::SetAngularVelocity(const glm::vec3 velocity);

    glm::quat AngularSpin() const;
    glm::mat3 InertiaTensor() const;
    glm::mat3 InvInertiaTensor() const;
    void SetInertiaLocal(glm::vec3 inertia);
    void ApplyTorque(glm::vec3 torque);
    /** Apply a force on the Node at a certain location, can generate torque */
    void ApplyLocalForce(glm::vec3 force, glm::vec3 forceLocation);
    /** Apply a force at the center of the object, won't generate torque */
    void ApplyCentralForce(glm::vec3 pushDirection);
    void ApplyCentralPush(glm::vec3 pushDirection);
    void ApplyLocalPush(glm::vec3 pushDirection, glm::vec3 pushLocation);
    void ApplyWorldPush(glm::vec3 pushDirection, glm::vec3 pushLocation, glm::vec3 originalPosition);
    void IntegrateVelocities(float step);
    void SetCollider(const std::shared_ptr<BoundingElement>& collider);

    std::shared_ptr<BoundingElement> GetCollider() const;
    Intersection Collides(const std::shared_ptr<RigidBody>& objectB);

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<RigidBody>(*this);
    }
    glm::vec3 _totalForce { 0 };
    glm::vec3 _totalTorque { 0 };

    glm::vec3 _linearFactor { 1 };
};

/*
inline glm::vec3 RigidBody::GetSupport(std::vector<glm::vec3> vertices, glm::vec3 d)
{
    float highest = std::numeric_limits<float>::lowest();
    glm::vec3 support = glm::vec3(0);

    for (const auto &v : vertices) {
        float dot = glm::dot(v, d);

        if (dot > highest) {
            highest = dot;
            support = v;
        }
    }
    return support;
}
*/

/*
class SphereImpostor : public RigidBody
{
public:
	static std::shared_ptr<SphereImpostor> Create(const std::string &name, std::shared_ptr<Node> node, const float radius);
	virtual Intersection IntersectRay(const Ray ray);
	virtual const std::vector<Ray> GatherRays();
	float Radius() const;
	void SetRadius(float radius);
private:
	float _radius { 0 };
	float _radius2 { 0 };
	SphereImpostor(const std::string &name);
};

class AABBImpostor : public RigidBody
{
public:
	static std::shared_ptr<SphereImpostor> Create(const std::string &name, std::shared_ptr<Node> node, const glm::vec3 size);
	virtual Intersection IntersectRay(const Ray ray);
	virtual const std::vector<Ray> GatherRays();
	virtual glm::vec3 Size() const;
	virtual void SetSize(glm::vec3 size);
    virtual glm::vec3 Min() const;
    virtual glm::vec3 Max() const;

private:
	glm::vec3 _size { 0 };
};

template <>
inline bool RigidBody::Collides(
        const std::shared_ptr<AABBImpostor>& a,
        const std::shared_ptr<AABBImpostor>& b)
{
    float d1x = b->Min().x - a->Max().x;
    float d1y = b->Min().y - a->Max().y;
    float d2x = a->Min().x - b->Max().x;
    float d2y = a->Min().y - b->Max().y;

    if (d1x > 0.0f || d1y > 0.0f)
        return false;

    if (d2x > 0.0f || d2y > 0.0f)
        return false;
    return true;
}

class BoxeImpostor : public AABBImpostor
{
public:
	static std::shared_ptr<RigidBody> Create(const std::string &name, std::shared_ptr<Node> node, const glm::vec3 size);
};
*/