/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:08
*/

#pragma once

#include "Component.hpp"
#include "Intersection.hpp"
//#include "Callback.hpp"
#include "Tools/Tools.hpp"
#include <glm/glm.hpp>
#include <limits>

class Ray;
class BoundingAABB;
class BoundingSphere;
class BoundingPlane;
class BoundingMesh;
class BoundingBox;
template <typename Signature>
class Callback;

#include "Debug.hpp"
#include <set>

struct compareVec {
    bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const
    {
        if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z < rhs.z)
            return true;
        if (lhs.x == rhs.x && lhs.y < rhs.y)
            return true;
        if (lhs.x < rhs.x)
            return true;
        return false;
    }
};

class BoundingElement : public Component {
public:
    enum class Type {
        Invalid = -1,
        Sphere,
        AABB,
        Plane,
        Geometry,
        Box,
        MaxType
    };
    struct CollisionEdge {
        glm::vec3 maximumProjection;
        glm::vec3 a;
        glm::vec3 b;
    };
    struct ProjectionInterval {
        ProjectionInterval(const glm::vec3& axis, const glm::vec3& min, const glm::vec3& max)
            : _axis(axis)
            , _min(min)
            , _max(max)
            , _minDot(dot(min, axis))
            , _maxDot(dot(max, axis))
        {
        }
        auto Axis() const { return _axis; }
        auto Min() const { return _min; }
        auto Max() const { return _max; }
        auto MinDot() const { return _minDot; }
        auto MaxDot() const { return _maxDot; }

    private:
        const glm::vec3 _axis { 0.f };
        const glm::vec3 _min { std::numeric_limits<float>::infinity() };
        const glm::vec3 _max { std::numeric_limits<float>::infinity() };
        const float _minDot { std::numeric_limits<float>::max() };
        const float _maxDot { std::numeric_limits<float>::lowest() };
        /*float start { std::numeric_limits<float>::max() };
        float end { std::numeric_limits<float>::lowest() };
        auto GetDistance(const ProjectionInterval& other)
        {
            if (other.end > start)
                return other.end - start;
            else if (end > other.start)
                return end - other.start;
            else {
                debugLog("the ProjectionIntervals overlap");
                return 0.f;
            }
        }
        auto Overlaps(const ProjectionInterval& other) { return end > other.start || start > other.end; }
        auto GetOverlap(const ProjectionInterval& other)
        {
            if (!Overlaps(other))
                return 0.f;
            if (end > other.start)
                return end - other.start;
            else if (start > other.end)
                return start - other.end;
            else {
                debugLog("the ProjectionIntervals don't overlap");
                return 0.f;
            }
        }
        auto Contains(const ProjectionInterval& other) { return start <= other.start && end >= other.end; }*/
    };
    BoundingElement(BoundingElement::Type);
    virtual BoundingElement::Type GetType() const final;

    virtual glm::mat3 LocalInertiaTensor(const float& mass) const = 0;
    virtual Intersection IntersectRay(const Ray& ray) const;
    virtual std::set<glm::vec3, compareVec> GetSATAxis(const glm::mat4& transform = glm::mat4(1.f)) const = 0;
    virtual ProjectionInterval Project(const glm::vec3& normal, const glm::mat4& transform = glm::mat4(1.f)) const = 0;
    //virtual glm::vec3 GetSupportPoint(const glm::vec3& axis, const glm::mat4& transform = glm::mat4(1.f)) const = 0;
    //virtual void GetMinMaxVertexOnAxis(const glm::vec3& axis, glm::vec3& min, glm::vec3& max, const glm::mat4& transform = glm::mat4(1.f)) const = 0;
    //virtual glm::vec3 GetLocalSupportPoint(glm::vec3 axis) const = 0;
    virtual std::vector<glm::vec3> Clip(glm::vec3 axis, const glm::mat4& transform = glm::mat4(1.f)) const = 0;
    //virtual CollisionEdge GetBestEdge(glm::vec3 axis, const glm::mat4 &transform = glm::mat4(1.f)) = 0;
    Intersection IntersectError(const std::shared_ptr<BoundingElement>& other) const;
    /*template <typename T, typename U,
    		  typename = IsBaseOf<BoundingElement, T>,
    		  typename = IsBaseOf<BoundingElement, U>>
	static Intersection Intersect(const T &a, const U &b);*/
    //void SetIntersectionCallback(BoundingElement::Type type, Intersection (*function)(const BoundingElement &));

private:
    /*virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<BoundingElement>(*this);
    }*/
    BoundingElement::Type _type { BoundingElement::Type::Invalid };
    //std::array<std::shared_ptr<Callback<Intersection(const BoundingElement &)>>, MaxType> _intersectionCallback;
    BoundingElement() = delete;
};
