/*
* @Author: gpinchon
* @Date:   2020-08-03 15:24:11
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-03 17:12:30
*/

#include "Physics/CollisionAlgorithmSAT.hpp"
#include "Physics/Collision.hpp"
#include "Physics/RigidBody.hpp"

bool CheckAxis(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, const glm::vec3 axis, Collision& out)
{
    auto collidees = Collision::CollideesPair(a, b);
    auto& transformA = a->GetNextTransform();
    auto& transformB = b->GetNextTransform();
    auto projectA = a->GetCollider()->Project(axis, transformA);
    auto projectB = b->GetCollider()->Project(axis, transformB);
    auto A = projectA.MinDot();
    auto B = projectA.MaxDot();
    auto C = projectB.MinDot();
    auto D = projectB.MaxDot();
    if (A <= C && B >= C) {
        auto normal = axis;
        auto penetration = C - B;
        auto position = projectA.Max() + normal * penetration;
        out = Collision(collidees, position, normal, penetration);
        return true;
    }
    if (C <= A && D >= A) {
        auto normal = -axis;
        auto penetration = A - D;
        auto position = projectA.Min() + normal * penetration;
        out = Collision(collidees, position, normal, penetration);
        return true;
    }
    return false;
}

bool CollisionAlgorithmSAT::Collides(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, Collision& out)
{

    auto boundingElementA(a->GetComponent<BoundingElement>());
    auto boundingElementB(b->GetComponent<BoundingElement>());
    if (boundingElementA == nullptr || boundingElementB == nullptr)
        return false;
    auto& transformA = a->GetNextTransform();
    auto& transformB = b->GetNextTransform();
    auto axisA(boundingElementA->GetSATAxis(transformA));
    auto axisB(boundingElementB->GetSATAxis(transformB));

    std::set<glm::vec3, compareVec> possibleAxis;

    for (const auto& norm1 : axisA) {
        for (const auto& norm2 : axisB) {
            possibleAxis.insert(cross(norm1, norm2));
        }
    }
    possibleAxis.insert(axisA.begin(), axisA.end());
    possibleAxis.insert(axisB.begin(), axisB.end());
    Collision bestCollision;
    for (const auto& axis : possibleAxis) {
        Collision collision;
        if (!CheckAxis(a, b, axis, collision)) //if true, we found separating Axis
            return false;
        if (bestCollision.PenetrationDepth() == std::numeric_limits<float>::infinity() || collision.PenetrationDepth() >= bestCollision.PenetrationDepth())
            bestCollision = collision;
    }
    std::cout << __FUNCTION__ << " D : " << bestCollision.PenetrationDepth() << std::endl;
    std::cout << __FUNCTION__ << " N : " << bestCollision.Normal().x << " " << bestCollision.Normal().y << " " << bestCollision.Normal().z << std::endl;
    std::cout << __FUNCTION__ << " P : " << bestCollision.Position().x << " " << bestCollision.Position().y << " " << bestCollision.Position().z << std::endl;
    //We haven't found a separating axis
    out = bestCollision;
    /*glm::vec3 overlapNormal;
    //loop through all the normals
    for (const auto& normal : axisA) {
        auto intervalA = boundingElementA->Project(normal, matrixA);
        auto intervalB = boundingElementB->Project(normal, matrixB);
        auto finalVelocity = dot(a->LinearVelocity() - b->LinearVelocity(), normal);
        auto velocityInterval = intervalA;
        if (finalVelocity > 0.f)
            velocityInterval.end += finalVelocity;
        else
            velocityInterval.start += finalVelocity;
        if (!intervalA.Overlaps(intervalB))
            return Intersection(false, intervalA.GetDistance(intervalB));
        auto overlap(intervalA.GetOverlap(intervalB));
        if (intervalA.Contains(intervalB) || intervalB.Contains(intervalA)) {
            double mins = abs(intervalA.start - intervalB.start);
            double maxs = abs(intervalA.end - intervalB.end);
            // NOTE: depending on which is smaller you may need to
            // negate the separating axis!!
            if (mins < maxs) {
                overlap += mins;
            } else {
                overlap += maxs;
            }
        }
        if (overlap < minOverlap) {
            // then set this one as the smallest
            minOverlap = overlap;
            overlapNormal = normal;
        }
    }*/
    return true;
}