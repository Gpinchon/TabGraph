#include "BoundingAABB.hpp"
#include "BoundingBox.hpp"
#include "BoundingMesh.hpp"
#include "BoundingPlane.hpp"
#include "BoundingSphere.hpp"
#include "Mesh/Geometry.hpp"
#include <glm/gtx/component_wise.hpp>

inline Intersection IntersectFunction(const glm::vec3& point, const BoundingAABB& box)
{
	auto underMax(glm::lessThanEqual(point, box.Max()));
	auto aboveMin(glm::greaterThanEqual(point, box.Min()));
	return Intersection(glm::all(underMax) && glm::all(aboveMin), 0.f);
}

inline Intersection IntersectFunction(const BoundingSphere& a, const BoundingSphere& b)
{
	float radiusDistance = a.GetRadius() + b.GetRadius();
	float centerDistance = glm::distance(a.GetCenter(), b.GetCenter());
	float distance = centerDistance - radiusDistance;
	return Intersection(centerDistance < radiusDistance, distance);
}

inline Intersection isPointInsideSphere(const glm::vec3& point, const BoundingSphere& sphere)
{
	auto distance(glm::distance(point, sphere.GetCenter()));
	return Intersection(distance < sphere.GetRadius(), distance);
}

inline Intersection IntersectFunction(const BoundingAABB& a, const BoundingAABB& b)
{
	auto distance1(b.Min() - a.Max());
	auto distance2(a.Min() - b.Max());
	auto distance(glm::max(distance1, distance2));
	auto maxDistance(glm::compMax(distance));
	return Intersection(maxDistance < 0, maxDistance);
}



/*
static inline bool IsMinowskiFace(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &d)
{
	auto BxA(glm::cross(b, a));
	auto DxC(glm::cross(d, c));
	auto CBA(glm::dot(c, BxA));
	auto DBA(glm::dot(d, BxA));
	auto ADC(glm::dot(a, DxC));
	auto BDC(glm::dot(b, DxC));
	return CBA * DBA < 0 && ADC * BDC < 0 && CBA * BDC > 0;
}

static inline bool BuildMinowskiFace(const glm::ivec2 &edgeA,  const glm::ivec2 &edgeB, const std::shared_ptr<Geometry> &meshA, const std::shared_ptr<Geometry> &meshB)
{
	auto a(meshA->GetVertex<glm::vec3>(Geometry::AccessorKey::Normal, edgeA[0]));
	auto b(meshA->GetVertex<glm::vec3>(Geometry::AccessorKey::Normal, edgeA[1]));
	auto c(meshA->GetVertex<glm::vec3>(Geometry::AccessorKey::Normal, edgeB[0]));
	auto d(meshA->GetVertex<glm::vec3>(Geometry::AccessorKey::Normal, edgeB[1]));
	return IsMinowskiFace(a, b, c, d);
}

struct EdgeQuery
{
	glm::ivec2 a;
	glm::ivec2 b;
	float distance;
};

static inline glm::vec3 GetEdgeDirection(const glm::ivec2 &edge, const std::shared_ptr<Geometry> &mesh)
{
	auto a(mesh->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, edge[0]));
	auto b(mesh->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, edge[1]));
	return glm::normalize(a - b);
}

static inline float Distance(const glm::ivec2 &edgeA, const glm::ivec2 &edgeB, const std::shared_ptr<Geometry> &meshA, const std::shared_ptr<Geometry> &meshB)
{
	auto edgeDirA(GetEdgeDirection(edgeA, meshA));
	auto pointA(meshA->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, edgeA[0]));
	auto edgeDirB(GetEdgeDirection(edgeB, meshB));
	auto pointB(meshB->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, edgeB[0]));

	if (glm::dot(edgeDirA, edgeDirB) == 0.f)
		return std::numeric_limits<float>::max();

	auto normal(glm::normalize(glm::cross(edgeDirA, edgeDirB)));
	if (glm::dot(normal, pointA - meshA->Centroid()))
		normal = -normal;
	return glm::dot(normal, pointB - pointA);
}

static inline std::vector<EdgeQuery> QueryEdgeDirection(const std::shared_ptr<Geometry> &meshA, const std::shared_ptr<Geometry> &meshB)
{
	std::vector<EdgeQuery> ret;
	for (auto indexA = 0u; indexA < meshA->EdgeCount(); ++indexA) {
		auto edgeA(meshA->GetEdge(indexA));
		for (auto indexB = 0u; indexB < meshB->EdgeCount(); ++indexB) {
			auto edgeB(meshB->GetEdge(indexB));
			if (BuildMinowskiFace(edgeA, edgeB, meshA, meshB)) {
				EdgeQuery query;
				query.a = edgeA;
				query.b = edgeB;
				query.distance = Distance(edgeA, edgeB, meshA, meshB);
				ret.push_back(query);
			}
		}
	}
	return ret;
}

inline Intersection IntersectFunction(const BoundingMesh &a, const BoundingMesh &b)
{

}
*/

#include "Physics/RigidBody.hpp"

/*auto Projection(const std::shared_ptr<Geometry> &geometry, const glm::mat4 &transform, glm::vec3 normal)
{
	BoundingElement::ProjectionInterval interval;
	for (auto index = 0u; index < geometry->VertexCount(); ++index) {
		auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, index));
		glm::vec3 transformedVertex(transform * glm::vec4(v, 1.f));
		auto dotProduct(glm::dot(transformedVertex, normal));
		interval.start = dotProduct < interval.start ? dotProduct : interval.start;
		interval.end = dotProduct > interval.end ? dotProduct : interval.end;
	}
	return interval;
}*/
