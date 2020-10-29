/*
* @Author: gpi
* @Date:   2019-05-15 13:53:32
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-03 19:10:40
*/

#include "Physics/BoundingAABB.hpp"
#include "Physics/Ray.hpp"

#include <array>
#include <glm/gtx/matrix_decompose.hpp>

BoundingAABB::BoundingAABB(glm::vec3 minV, glm::vec3 maxV)
    : BoundingElement(Type::AABB)
    , _min(minV)
    , _max(maxV)
{
}

std::shared_ptr<BoundingAABB> BoundingAABB::Create(glm::vec3 min, glm::vec3 max)
{
    return tools::make_shared<BoundingAABB>(min, max);
}

glm::vec3 BoundingAABB::Min() const
{
    return _min;
}

void BoundingAABB::SetMin(glm::vec3 min)
{
    _min = min;
}

glm::vec3 BoundingAABB::Max() const
{
    return _max;
}

void BoundingAABB::SetMax(glm::vec3 max)
{
    _max = max;
}

/*#include <glm/gtx/component_wise.hpp>

Intersection BoundingAABB::IntersectAABB(const std::shared_ptr<BoundingAABB>& other) const
{
    return IntersectFunction(*this, *other);
}*/

std::set<glm::vec3, compareVec> BoundingAABB::GetSATAxis(const glm::mat4& /*transform*/) const
{
    static std::set<glm::vec3, compareVec> axis({ glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1),
        glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1) });
    return axis;
}

glm::mat3 BoundingAABB::LocalInertiaTensor(const float& mass) const
{
    auto w = fabs(Max().x - Min().x);
    auto w2 = w * w;
    auto h = fabs(Max().y - Min().y);
    auto h2 = h * h;
    auto d = fabs(Max().z - Min().z);
    auto d2 = d * d;
    auto m = 1 / 12.f * mass;
    return {
        m * (h2 + d2), 0, 0,
        0, m * (w2 + d2), 0,
        0, 0, m * (w2 + h2)
    };
}

BoundingElement::ProjectionInterval BoundingAABB::Project(const glm::vec3& axis, const glm::mat4& transform) const
{
    auto vec = axis;
    if (length(vec) < 0.0001) {
        vec = glm::vec3(1, 0, 0);
    } else
        vec = normalize(vec);
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    //std::cout << "axis : " << vec.x << ' ' << vec.y << ' ' << vec.z << '\n';
    //std::cout << "min  : " << Min().x << ' ' << Min().y << ' ' << Min().z << '\n';
    //std::cout << "max  : " << Max().x << ' ' << Max().y << ' ' << Max().z << '\n';
    {
        static std::array<glm::vec3, 8> vertex;
        vertex.at(0) = Max() + translation;
        vertex.at(1) = glm::vec3(Min().x, Min().y, Max().z) + translation;
        vertex.at(2) = glm::vec3(Min().x, Max().y, Max().z) + translation;
        vertex.at(3) = glm::vec3(Min().x, Max().y, Min().z) + translation;
        vertex.at(4) = glm::vec3(Max().x, Max().y, Min().z) + translation;
        vertex.at(5) = glm::vec3(Max().x, Min().y, Min().z) + translation;
        vertex.at(6) = glm::vec3(Max().x, Min().y, Max().z) + translation;
        vertex.at(7) = Min() + translation;
        glm::vec3 minV;
        float minDot { std::numeric_limits<float>::max() };
        glm::vec3 maxV;
        float maxDot { std::numeric_limits<float>::lowest() };
        for (auto v : vertex) {
            auto dotProd = dot(v, vec);
            if (dotProd < minDot) {
                minDot = dotProd;
                minV = v;
            }
            if (dotProd > maxDot) {
                maxDot = dotProd;
                maxV = v;
            }
        }
        /*std::cout << "BruteForce :\n";
        std::cout << "minV : " << minV.x << ' ' << minV.y << ' ' << minV.z << '\n';
        std::cout << "maxV : " << maxV.x << ' ' << maxV.y << ' ' << maxV.z << '\n';*/
        return ProjectionInterval(vec, minV, maxV);
    }
    /*auto position = (Max() + Min()) * 0.5f;
    auto size = (Max() - Min());
    auto half = size * 0.5f;
    auto minV = glm::vec3(
                    (vec.x < 0) ? half.x : -half.x,
                    (vec.y < 0) ? half.y : -half.y,
                    (vec.z < 0) ? half.z : -half.z)
        + position + translation;
    auto maxV = glm::vec3(
                    (vec.x < 0) ? -half.x : half.x,
                    (vec.y < 0) ? -half.y : half.y,
                    (vec.z < 0) ? -half.z : half.z)
        + position + translation;
    std::cout << "Smartypants :\n";
    std::cout << " half : " << half.x << ' ' << half.y << ' ' << half.z << '\n';
    std::cout << " posi : " << position.x << ' ' << position.y << ' ' << position.z << '\n';
    std::cout << "minV : " << minV.x << ' ' << minV.y << ' ' << minV.z << '\n';
    std::cout << "maxV : " << maxV.x << ' ' << maxV.y << ' ' << maxV.z << '\n';
    std::cout << std::endl;
    return ProjectionInterval(vec, minV, maxV);*/
}

static inline auto IsInFront(glm::vec3 axis, glm::vec3 point)
{
    return dot(axis, point) > 0;
}

static inline auto IntersectionPoint(glm::vec3 axis, glm::vec3 a, glm::vec3 b)
{
    auto dir(normalize(a - b));
    float denom(dot(axis, dir));
    auto p(-a);
    auto t(dot(p, axis) / denom);
    return a + t * dir;
}

std::vector<glm::vec3> BoundingAABB::Clip(glm::vec3 axis, const glm::mat4& transform) const
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    std::vector<glm::vec3> output;
    static std::array<glm::vec3, 7> vertex;

    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    vertex.at(0) = Max() + translation;
    vertex.at(1) = glm::vec3(Min().x, Min().y, Max().z) + translation;
    vertex.at(2) = glm::vec3(Min().x, Max().y, Max().z) + translation;
    vertex.at(3) = glm::vec3(Min().x, Max().y, Min().z) + translation;
    vertex.at(4) = glm::vec3(Max().x, Max().y, Min().z) + translation;
    vertex.at(5) = glm::vec3(Max().x, Min().y, Min().z) + translation;
    vertex.at(6) = glm::vec3(Max().x, Min().y, Max().z) + translation;
    auto startingPoint(vertex.at(6));
    for (const auto& v : vertex) {
        glm::vec3 endPoint(glm::vec4(v, 1.f));
        if (IsInFront(axis, startingPoint) && IsInFront(axis, endPoint)) {
            output.push_back(startingPoint);
            output.push_back(endPoint);
        } else if (IsInFront(axis, startingPoint) && !IsInFront(axis, endPoint)) {
            output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
            output.push_back(startingPoint);
        } else if (!IsInFront(axis, startingPoint) && IsInFront(axis, endPoint)) {
            output.push_back(IntersectionPoint(axis, endPoint, startingPoint));
            output.push_back(endPoint);
        }
        startingPoint = endPoint;
    }
    return output;
}

static inline Intersection CreateAABBRayIntersection(const bool intersects, const float distance, const BoundingAABB* aabb, const Ray& ray)
{
    auto position = ray.direction * distance + ray.origin;
    auto c = (aabb->Min() + aabb->Max()) * 0.5f;
    auto d = (aabb->Min() - aabb->Max()) * 0.5f;
    auto p = position - c;
    auto bias = 1.000001;
    auto normal = glm::vec3(p.x / abs(d.x) * bias,
        p.y / abs(d.y) * bias,
        p.z / abs(d.z) * bias);
    normal = normalize(normal);
    return Intersection(intersects, distance, normal);
}

Intersection BoundingAABB::IntersectRay(const Ray& ray) const
{
    /*glm::vec3 min = (Min() - ray.origin) / ray.direction;
    glm::vec3 max = (Max() - ray.origin) / ray.direction;
    float tmin = glm::compMin(min);
    float tmax = glm::compMin(max);
    if (tmin > tmax)
        return intersection;*/
    float tmin = (Min().x - ray.origin.x) / ray.direction.x;
    float tmax = (Max().x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax)
        std::swap(tmin, tmax);

    float tymin = (Min().y - ray.origin.y) / ray.direction.y;
    float tymax = (Max().y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return CreateAABBRayIntersection(false, tmin, this, ray);

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (Min().z - ray.origin.z) / ray.direction.z;
    float tzmax = (Max().z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return CreateAABBRayIntersection(false, tmin, this, ray);
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return CreateAABBRayIntersection(true, tmin, this, ray);
}