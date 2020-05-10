/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:16:56
*/

#pragma once

#include "Intersection.hpp"
//#include "Callback.hpp"
#include "Tools.hpp"
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

//typedef Intersection (*IntersectCallback)(const BoundingBox &);

class BoundingElement {
public:
	enum Type
	{
		Invalid = -1,
		Sphere,
		AABB,
		Plane,
		Mesh,
		Box,
		MaxType
	};
    BoundingElement(BoundingElement::Type);
    virtual BoundingElement::Type GetType() const final;
    virtual Intersection Intersect(const std::shared_ptr<BoundingElement> &other) const final;
    virtual Intersection IntersectRay(const Ray &ray) const;
    virtual Intersection IntersectSphere(const std::shared_ptr<BoundingSphere> &other) const;
    virtual Intersection IntersectAABB(const std::shared_ptr<BoundingAABB> &other) const;
    virtual Intersection IntersectPlane(const std::shared_ptr<BoundingPlane> &other) const;
    virtual Intersection IntersectMesh(const std::shared_ptr<BoundingMesh> &other) const;
    virtual Intersection IntersectBox(const std::shared_ptr<BoundingBox> &other) const;
    virtual void Transform(const glm::vec3 &translation, const glm::quat &rotation, const glm::vec3 &scale);
	Intersection IntersectError(const std::shared_ptr<BoundingElement> &other) const;
	    /*template <typename T, typename U,
    		  typename = IsBaseOf<BoundingElement, T>,
    		  typename = IsBaseOf<BoundingElement, U>>
	static Intersection Intersect(const T &a, const U &b);*/
    //void SetIntersectionCallback(BoundingElement::Type type, Intersection (*function)(const BoundingElement &));

private:
	BoundingElement::Type _type { BoundingElement::Type::Invalid };
	//std::array<std::shared_ptr<Callback<Intersection(const BoundingElement &)>>, MaxType> _intersectionCallback;
	BoundingElement() = delete;
};


