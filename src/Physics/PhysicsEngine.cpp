#include "Physics/PhysicsEngine.hpp"
#include "Node.hpp"
#include "Physics/IntersectFunctions.hpp"
#include "Physics/RigidBody.hpp"
#include <algorithm>
#include <glm/gtx/norm.hpp>

void PhysicsEngine::AddRigidBody(const std::shared_ptr<RigidBody>& rigidBody)
{
    _rigidBodies.push_back(rigidBody);
}

float lastStep = 0.f;

void SimulateBody(std::shared_ptr<RigidBody> rigidBody, float step)
{
    rigidBody->IntegrateVelocities(step);
    auto& currTransform(rigidBody->CurrentTransform());
    auto& nextTransform(rigidBody->NextTransform());
    auto node(rigidBody->GetNode());
    if (node != nullptr && node->GetComponent<Transform>() != nullptr) {
        currTransform.SetPosition(node->GetComponent<Transform>()->WorldPosition());
        currTransform.SetRotation(node->GetComponent<Transform>()->WorldRotation());
    }
    nextTransform.SetPosition(currTransform.Position() + rigidBody->LinearVelocity() * step);
    nextTransform.SetRotation(normalize(currTransform.Rotation() + step * 0.5f * rigidBody->AngularSpin() * currTransform.Rotation()));
}

void PhysicsEngine::Simulate(float step)
{
    for (auto& rigidBody : _rigidBodies) {
        SimulateBody(rigidBody, step);
    }
    lastStep = step;
}

#include "Physics/Collision.hpp"
#include <iostream>

static std::vector<Collision> collisionList;
static std::vector<Collision> cachedCollisionList;

void SignalCollision(const Collision& collision)
{
    if (std::find(collisionList.begin(), collisionList.end(), collision) == collisionList.end()) {
        collisionList.push_back(collision);
    }
}

auto PointVelocity(std::shared_ptr<RigidBody> a, const glm::vec3& p)
{
    auto r = p - a->NextTransform().WorldPosition();
    return a->LinearVelocity() + cross(a->AngularVelocity(), r);
}

enum CollisionType {
    MovingAway,
    Resting,
    Colliding
};

auto colliding(const Collision& c)
{
    auto pair = c.Collidees();
    auto a = pair.First();
    auto b = pair.Second();
    auto vp1 = PointVelocity(a, c.Position()), vp2 = PointVelocity(b, c.Position());

    auto vrel = dot(c.Normal(), (vp1 - vp2));

    //std::cout << "vrel : " << vrel << '\n';

    if (vrel > 0.0001) /* moving away */ {
        //std::cout << " moving away\n";
        return MovingAway;
    }
    if (vrel > -0.0001) /* resting contact */ {
        //std::cout << " resting contact\n";
        return Resting;
    } else /* vrel < -THRESHOLD */ {
        //std::cout << " colliding\n";
        return Colliding;
    }
}

auto computeImpulseDenominator(std::shared_ptr<RigidBody> rigidBody, const glm::vec3& pos, const glm::vec3& normal)
{
    auto r0 = pos - rigidBody->NextTransform().WorldPosition();
    auto c0 = cross(r0, normal);
    auto vec = cross(c0 * rigidBody->InvInertiaTensor(), r0);

    return rigidBody->InvMass() + dot(normal, vec);
}

std::map<std::shared_ptr<RigidBody>, std::pair<glm::vec3, glm::vec3>> totalImpulses;

void HandleCollision(Collision collision)
{
    //if (!colliding(collision))
    //    continue;
    auto pair = collision.Collidees();
    auto a = pair.First();
    auto b = pair.Second();

    ////std::cout << a->Name() << ' ' << b->Name() << '\n';
    ////std::cout << "n : " << collision.Normal().x << ' ' << collision.Normal().y << ' ' << collision.Normal().z << '\n';
    ////std::cout << "p : " << collision.Position().x << ' ' << collision.Position().y << ' ' << collision.Position().z << '\n';
    auto& transformA = a->NextTransform();
    auto& transformB = b->NextTransform();

    auto n = collision.Normal();
    auto e = std::min(a->Restitution(), b->Restitution());

    auto I1 = a->InvInertiaTensor();
    auto I2 = b->InvInertiaTensor();
    auto w1 = a->AngularVelocity();
    auto w2 = b->AngularVelocity();
    auto v1 = a->LinearVelocity();
    auto v2 = b->LinearVelocity();
    auto r1 = collision.Position() - transformA.WorldPosition();
    auto r2 = collision.Position() - transformB.WorldPosition();
    auto vp1 = v1 + cross(w1, r1);
    auto vp2 = v2 + cross(w2, r2);
    auto vr = vp2 - vp1;
    auto ri1 = cross(I1 * cross(r1, n), r1);
    auto ri2 = cross(I2 * cross(r2, n), r2);
    auto im1 = a->InvMass();
    auto im2 = b->InvMass();
    auto ja = -(1.f + e) * dot(vr, n);
    auto jb = im1 + im2 + dot(ri1 + ri2, n);
    auto j = ja / jb;

    /*auto pa = transformA.WorldPosition() + n * collision.Fraction();
    auto pb = transformB.WorldPosition() + -n * collision.Fraction();
    auto penetrationDepth = dot(pb - pa, -n);*/
    //j = 0.f;
    /*auto penetrationDepth = collision.PenetrationDepth();
    //std::cout << "penetrationDepth   : " << penetrationDepth << '\n';
    const auto baumgarteScalar = 0.01f;
    const auto baumgarteSlop = 0.00001f;
    const auto penetrationSlop = std::min(penetrationDepth + baumgarteSlop, 0.f);
    auto correction = -(baumgarteScalar / lastStep) * penetrationSlop;
    //std::cout << "correction : " << correction << '\n';
    j += std::max(0.f, correction);*/
    //std::cout << "j   " << j << '\n';
    //std::cout << "ja  " << ja << '\n';
    //std::cout << "jb  " << jb << '\n';
    //std::cout << "im1 " << im1 << '\n';
    //std::cout << "im2 " << im2 << '\n';
    //std::cout << dot(ri1 + ri2, n) << '\n';

    //j *= 10.f;

    //auto jr = j * n;

    if (!a->Static()) {
        //a->ApplyLocalPush(jr, r1);
        auto v = v1 - j * im1 * n;
        auto w = w1 - j * I1 * cross(r1, n);
        a->SetLinearVelocity(v);
        a->SetAngularVelocity(w);
        //totalImpulses[a].first += -j * im1 * n;
        //totalImpulses[a].second += -j * I1 * cross(r1, n);
    }

    if (!b->Static()) {
        //b->ApplyLocalPush(-jr, r2);
        auto v = v2 + j * im2 * n;
        auto w = w2 + j * I2 * cross(r2, n);
        b->SetLinearVelocity(v);
        b->SetAngularVelocity(w);
        //totalImpulses[b].first += j * im2 * n;
        //totalImpulses[b].second += j * I2 * cross(r2, n);
    }
}
/*
auto ComputeAij(const Collision& ci, const Collision& cj)
{
    auto cia = ci.Collidees().First();
    auto cib = ci.Collidees().Second();
    auto cja = cj.Collidees().First();
    auto cjb = cj.Collidees().Second();
    if (cia != cja && cib != cjb && cia != cjb && cib != cja)
        return 0.f;
    auto A = cia;
    auto B = cib;
    auto Ax = A->NextTransform().WorldPosition();
    auto Bx = B->NextTransform().WorldPosition();
    auto ni = ci.Normal();
    auto nj = cj.Normal();
    auto pi = ci.Position();
    auto pj = cj.Position();
    auto ra = pi - Ax;
    auto rb = pi - Bx;
    auto forceOnA = glm::vec3(0.f);
    auto torqueOnA = glm::vec3(0.f);
    if (cja == cia) {
        forceOnA = nj;
        torqueOnA = cross(pj - Ax, nj);
    } else if (cjb == cia) {
        forceOnA = -nj;
        torqueOnA = cross(pj - Ax, nj);
    }
    auto forceOnB = glm::vec3(0.f);
    auto torqueOnB = glm::vec3(0.f);
    if (cja == cib) {
        forceOnA = nj;
        torqueOnB = cross(pj - Bx, nj);
    } else if (cjb == cib) {
        forceOnB = -nj;
        torqueOnB = cross(pj - Bx, nj);
    }
    auto aLinear = forceOnA / A->Mass();
    auto aAngular = (A->InvInertiaTensor() * torqueOnA) * ra;
    auto bLinear = forceOnB / B->Mass();
    auto bAngular = (B->InvInertiaTensor() * torqueOnB) * rb;
    return dot(ni, (aLinear + aAngular) - (bLinear + bAngular));
}

void ComputeAMatrix(const std::vector<Collision>& contacts, std::vector<std::vector<float>>& A)
{
    for (auto i = 0u; i < contacts.size(); ++i) {
        for (auto j = 0u; j < contacts.size(); ++j) {
            A[i][j] = ComputeAij(contacts[i], contacts[j]);
        }
    }
}

void ComputeBVector(const std::vector<Collision>& contacts, std::vector<float>& b)
{
    for (auto i = 0u; i < contacts.size(); ++i) {
        const auto c = contacts[i];
        auto A = c.Collidees().First();
        auto B = c.Collidees().Second();
        auto Ax = A->NextTransform().WorldPosition();
        auto Bx = B->NextTransform().WorldPosition();
        auto n = c.Normal(), // nˆi(t0 )
            ra = c.Position() - Ax, // p − xa(t0 ) 
            rb = c.Position() - Bx; // p − xb(t0 )
        // Get the external forces and torques
        auto f_ext_a = A->TotalForce(),
             f_ext_b = B->TotalForce(),
             t_ext_a = A->TotalTorque(),
             t_ext_b = B->TotalTorque();
        glm::vec3 a_ext_part, a_vel_part, b_ext_part, b_vel_part;
        // Operators: `' is for cross product, `*', is for
        //dot products (between two triples), or matrix-vector
        //multiplication (between a matrix and a triple).
        // Compute the part of p¨a(t0 ) due to the external
        //force and torque, and similarly for p¨b(t0 ). 
        a_ext_part = f_ext_a / A->Mass() + cross((A->InvInertiaTensor() * t_ext_a), ra),
        b_ext_part = f_ext_b / B->Mass() + cross((B->InvInertiaTensor() * t_ext_b), rb);
        // Compute the part of p¨a(t0 ) due to velocity, and similarly for p¨b(t0 ). 
        //(A->LinearVelocity() * A->AngularVelocity()) <- IS THIS A DOT ?!
        a_vel_part = cross(A->AngularVelocity(), cross(A->AngularVelocity(), ra)) + cross((A->InvInertiaTensor() * (A->LinearVelocity() * A->AngularVelocity())), ra);
        b_vel_part = cross(B->AngularVelocity(), cross(B->AngularVelocity(), rb)) + cross((B->InvInertiaTensor() * (B->LinearVelocity() * B->AngularVelocity())), rb);
        // Combine the above results, and dot with nˆi(t0)
        auto k1 = dot(n, ((a_ext_part + a_vel_part) - (b_ext_part + b_vel_part)));
        auto ndot = cross(A->AngularVelocity(), n); 
        ComputeNdot(c);
        //See section 8 for `pt_velocity' definition
        auto k2 = 2.f * dot(ndot, PointVelocity(A, c.Position()) - PointVelocity(B, c.Position()));
        b[i] = k1 + k2;
    }
}

void ComputeContactForces(const std::vector<Collision>& contacts)
{
    auto A = std::vector<std::vector<float>>(contacts.size(), std::vector<float>(contacts.size(), 0.f));
    auto b = std::vector<float>(contacts.size());
    auto f = std::vector<float>(contacts.size());
    ComputeAMatrix(contacts, A);
    ComputeBVector(contacts, b);
}*/

void HandleCollisions()
{
    //std::vector<Collision> restingContacts;
    bool collisionFound;
    do {
        collisionFound = false;
        for (const auto& collision : collisionList) {
            auto collisionType = colliding(collision);
            if (collisionType == Colliding) {
                collisionFound = true;
                HandleCollision(collision);
            } else if (collisionType == Resting) {
                //restingContacts.push_back(collision);
                HandleCollision(collision);
            }
        }
        for (auto impulse : totalImpulses) {
            auto a = impulse.first;
            a->SetLinearVelocity(a->LinearVelocity() + impulse.second.first);
            a->SetAngularVelocity(a->AngularFactor() + impulse.second.second);
            //a->NextTransform().SetPosition(a->NextTransform().Position() + a->LinearVelocity() * lastStep);
            //a->NextTransform().SetRotation(normalize(a->NextTransform().Rotation() + lastStep * 0.5f * a->AngularSpin() * a->NextTransform().Rotation()));
            //a->IntegrateVelocities(lastStep);
        }
        totalImpulses.clear();
    } while (collisionFound);

    /*for (const auto& collision : collisionList)
        HandleCollision(collision);
    for (auto impulse : totalImpulses) {
        auto a = impulse.first;
        a->SetLinearVelocity(a->LinearVelocity() + impulse.second.first);
        a->SetAngularVelocity(a->AngularFactor() + impulse.second.second);
        a->NextTransform().SetPosition(a->CurrentTransform().Position() + a->LinearVelocity() * lastStep);
        a->NextTransform().SetRotation(normalize(a->CurrentTransform().Rotation() + lastStep * 0.5f * a->AngularSpin() * a->CurrentTransform().Rotation()));
        //impulse.first->IntegrateVelocities(lastStep);
    }*/
    totalImpulses.clear();
    collisionList.clear();
}

#include "Physics/CollisionAlgorithmGJKRaycast.hpp"
#include "Physics/CollisionAlgorithmSAT.hpp"

void PhysicsEngine::CheckCollision()
{
    CollisionAlgorithmGJKRaycast collisionAlgorithm;
    for (auto& a : _rigidBodies) {
        for (auto& b : _rigidBodies) {
            if (a == b)
                continue;
            Collision::CollideesPair pair(a, b);
            if (std::find(_collideesPairs.begin(), _collideesPairs.end(), pair) != _collideesPairs.end())
                continue;
            // auto intersectionResult(a->Collides(b));
            // auto intersection(a->Collides(b));

            //auto intersection = GJKIntersection(a, b);
            //IntersectionResult intersectionResult;
            //auto intersects = GJKRayCast(a, b, intersectionResult);
            Collision out;
            while (collisionAlgorithm.Collides(a, b, out) && colliding(out) != Resting) {
                _collideesPairs.push_back(pair);
                //SignalCollision(out);
                auto finalFraction = collisionAlgorithm.GetFraction() - 0.1f;
                if (!a->Static()) {
                    /*auto pos = mix(a->CurrentTransform().WorldPosition(), a->NextTransform().WorldPosition(), collisionAlgorithm.GetFraction());
                    auto dist = distance(pos, a->NextTransform().WorldPosition());
                    a->NextTransform().SetPosition(pos + out.Normal() * dist);*/
                    //auto dir = a->NextTransform().WorldPosition() - a->CurrentTransform().WorldPosition();
                    //a->NextTransform().SetPosition(a->CurrentTransform().WorldPosition() + dir * finalFraction);
                    a->NextTransform().SetPosition(mix(a->CurrentTransform().WorldPosition(), a->NextTransform().WorldPosition(), finalFraction));
                    a->NextTransform().SetRotation(mix(a->CurrentTransform().WorldRotation(), a->NextTransform().WorldRotation(), finalFraction));
                }
                if (!b->Static()) {
                    /*auto pos = mix(b->CurrentTransform().WorldPosition(), b->NextTransform().WorldPosition(), collisionAlgorithm.GetFraction());
                    auto dist = distance(pos, b->NextTransform().WorldPosition());
                    b->NextTransform().SetPosition(pos - out.Normal() * dist);*/
                    //auto dir = b->NextTransform().WorldPosition() - b->CurrentTransform().WorldPosition();
                    //b->NextTransform().SetPosition(b->CurrentTransform().WorldPosition() + dir * finalFraction);
                    b->NextTransform().SetPosition(mix(b->CurrentTransform().WorldPosition(), b->NextTransform().WorldPosition(), finalFraction));
                    b->NextTransform().SetRotation(mix(b->CurrentTransform().WorldRotation(), b->NextTransform().WorldRotation(), finalFraction));
                }
                HandleCollision(out);
                CheckCollision();
                return;
                /*if (!a->Static()) {
                    a->NextTransform().SetPosition(a->NextTransform().Position() + a->LinearVelocity() * lastStep);
                    a->NextTransform().SetRotation(normalize(a->NextTransform().Rotation() + lastStep * 0.5f * a->AngularSpin() * a->NextTransform().Rotation()));
                }
                if (!b->Static()) {
                    b->NextTransform().SetPosition(b->NextTransform().Position() + b->LinearVelocity() * lastStep);
                    b->NextTransform().SetRotation(normalize(b->NextTransform().Rotation() + lastStep * 0.5f * b->AngularSpin() * b->NextTransform().Rotation()));
                }*/

                //return;
            }
            /*if (intersects && intersectionResult.Fraction() <= 1 && length2(intersectionResult.Normal()) != 0.f) {
                _collideesPairs.push_back(pair);
                SignalCollision(pair, intersectionResult);
            }*/
        }
    }
    /*for (auto impulse : totalImpulses) {
        auto a = impulse.first;
        a->SetLinearVelocity(a->LinearVelocity() + impulse.second.first);
        a->SetAngularVelocity(a->AngularFactor() + impulse.second.second);
        //a->NextTransform().SetPosition(a->NextTransform().Position() + a->LinearVelocity() * lastStep);
        //a->NextTransform().SetRotation(normalize(a->NextTransform().Rotation() + lastStep * 0.5f * a->AngularSpin() * a->NextTransform().Rotation()));
        //a->IntegrateVelocities(lastStep);
    }
    totalImpulses.clear();*/
    //HandleCollisions();
    //Simulate(lastStep);
    for (auto& rigidBody : _rigidBodies) {
        rigidBody->GetComponent<Node>()->GetComponent<Transform>()->SetPosition(rigidBody->NextTransform().WorldPosition());
        rigidBody->GetComponent<Node>()->GetComponent<Transform>()->SetRotation(normalize(rigidBody->NextTransform().WorldRotation()));
    }
    _collideesPairs.clear();
}