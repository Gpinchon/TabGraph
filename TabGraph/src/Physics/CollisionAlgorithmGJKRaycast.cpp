/*
* @Author: gpinchon
* @Date:   2020-08-03 16:05:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-06 19:57:04
*/

#define SIMD_EPSILON 0.0001

#include "Physics/CollisionAlgorithmGJKRaycast.hpp"
#include "Physics/Collision.hpp"
#include "Physics/RigidBody.hpp"
#include "Physics/SimplexSolver.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

glm::mat4 CreateTransformMatrix(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
{
    return glm::translate(position) * glm::mat4(rotation) * glm::scale(scale);
}

bool CollisionAlgorithmGJKRaycast::Collides(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, Collision& out)
{
    auto colliderA(a->GetCollider());
    auto colliderB(b->GetCollider());

    SimplexSolver simplex;

    glm::vec3 linVelA, linVelB;
    linVelA = a->GetNextPosition() - a->GetCurrentPosition();
    linVelB = b->GetNextPosition() - b->GetCurrentPosition();

    float lambda = float(0.);

    auto interpolatedPositionA{ a->GetCurrentPosition() };
    auto interpolatedRotationA{ a->GetCurrentRotation() };
    auto interpolatedScaleA{ a->GetCurrentScale() };
    auto interpolatedPositionB{ b->GetCurrentPosition() };
    auto interpolatedRotationB{ b->GetCurrentRotation() };
    auto interpolatedScaleB{ b->GetCurrentScale() };

    ///take relative motion
    glm::vec3 r = linVelA - linVelB;
    glm::vec3 v;

    glm::vec3 supVertexA = colliderA->Project(-r, a->GetCurrentTransform()).Max();
    glm::vec3 supVertexB = colliderB->Project(r, b->GetCurrentTransform()).Max();
    v = supVertexA - supVertexB;
    int maxIter = 32;

    glm::vec3 n = glm::vec3(0.f);
    bool hasResult = false;
    //glm::vec3 c;

    //float lastLambda = lambda;

    float dist2 = length2(v);
    float epsilon = float(0.0001);
    glm::vec3 w /*, p*/;
    float VdotR;

    while ((dist2 > epsilon) && maxIter--) {
        supVertexA = colliderA->Project(-v, CreateTransformMatrix(interpolatedPositionA, interpolatedRotationA, interpolatedScaleA)).Max();
        supVertexB = colliderB->Project(v, CreateTransformMatrix(interpolatedPositionB, interpolatedRotationB, interpolatedScaleB)).Max();
        w = supVertexA - supVertexB;

        float VdotW = dot(v, w);

        if (lambda > 1.f) {
            return false;
        }

        if (VdotW > 0.f) {
            VdotR = dot(v, r);

            if (VdotR >= -(SIMD_EPSILON * SIMD_EPSILON))
                return false;
            else {
                lambda = lambda - VdotW / VdotR;
                //interpolate to next lambda
                //  x = s + lambda * r;

                interpolatedPositionA = glm::mix(a->GetCurrentPosition(), a->GetNextPosition(), lambda);
                interpolatedRotationA = glm::mix(a->GetCurrentRotation(), a->GetNextRotation(), lambda);
                interpolatedScaleA = glm::mix(a->GetCurrentScale(), a->GetNextScale(), lambda);
                interpolatedPositionB = glm::mix(b->GetCurrentPosition(), b->GetNextPosition(), lambda);
                interpolatedRotationB = glm::mix(b->GetCurrentRotation(), b->GetNextRotation(), lambda);
                interpolatedScaleB = glm::mix(b->GetCurrentScale(), b->GetNextScale(), lambda);
                //simplex.reset();
                //check next line
                w = supVertexA - supVertexB;
                //lastLambda = lambda;
                n = v;
                hasResult = true;
            }
        }
        ///Just like regular GJK only add the vertex if it isn't already (close) to current vertex, it would lead to divisions by zero and NaN etc.
        if (!simplex.InSimplex(w))
            simplex.AddVertex(w, supVertexA, supVertexB);

        if (simplex.Closest(v)) {
            dist2 = length2(v);
            //hasResult = true;
            //todo: check this normal for validity
            //n=v;
            //printf("V=%f , %f, %f\n",v[0],v[1],v[2]);
            //printf("DIST2=%f\n",dist2);
            //printf("numverts = %i\n",simplex.numVertices());
        } else {
            dist2 = 0.f;
        }
    }

    //int numiter = MAX_ITERATIONS - maxIter;
    //  printf("number of iterations: %d", numiter);

    if (!hasResult) {
        n = v;
    }
    //don't report a time of impact when moving 'away' from the hitnormal

    if (length2(n) > (SIMD_EPSILON * SIMD_EPSILON))
        _intersectionResult.SetNormal(normalize(n));
    else
        _intersectionResult.SetNormal(glm::vec3(0.f));

    //don't report time of impact for motion away from the contact normal (or causes minor penetration)
    if (dot(_intersectionResult.Normal(), r) >= -AllowedPenetration() || lambda > 1) {
        return false;
    }

    glm::vec3 hitA, hitB;
    simplex.ComputePoints(hitA, hitB);
    auto ab = hitA - hitB;
    auto d = -dot(ab, _intersectionResult.Normal());
    _intersectionResult.SetFraction(lambda);
    _intersectionResult.SetHitPointA(hitA);
    _intersectionResult.SetHitPointB(hitB);
    _intersectionResult.SetPenetrationDepth(d);
    //std::cout << "ab : " << ab.x << ' ' << ab.y << ' ' << ab.z << '\n';
    //std::cout << "n  : " << _intersectionResult.Normal().x << ' ' << _intersectionResult.Normal().y << ' ' << _intersectionResult.Normal().z << '\n';
    //std::cout << "d  : " << d << '\n';
    //std::cout << glm::distance(hitA, hitB) << '\n';
    out = Collision(Collision::CollideesPair(a, b), _intersectionResult.HitPoint(), _intersectionResult.Normal(), _intersectionResult.PenetrationDepth());
    return true;
    //return length2(n) > 0;
}

float CollisionAlgorithmGJKRaycast::AllowedPenetration() const
{
    return _allowedPenetration;
}

void CollisionAlgorithmGJKRaycast::SetAllowedPenetration(float allowedPenetration)
{
    _allowedPenetration = allowedPenetration;
}

float CollisionAlgorithmGJKRaycast::GetFraction() const
{
    return _intersectionResult.Fraction();
}