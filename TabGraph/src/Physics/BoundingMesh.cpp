/*
* @Author: gpinchon
* @Date:   2020-05-01 21:59:49
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:53:16
*/

#include "Physics/BoundingMesh.hpp"
#include "Mesh/Geometry.hpp"
#include "Mesh/Mesh.hpp"

BoundingMesh::BoundingMesh(const std::shared_ptr<Mesh>& geometry)
    : BoundingElement(Geometry)
{
    SetMesh(geometry);
}

std::shared_ptr<Mesh> BoundingMesh::GetMesh() const
{
    return GetComponent<Mesh>();
}

void BoundingMesh::SetMesh(const std::shared_ptr<Mesh>& geometry)
{
    SetComponent(geometry);
}

#include "Buffer/BufferAccessor.hpp"

std::set<glm::vec3, compareVec> BoundingMesh::GetSATAxis(const glm::mat4& transform) const
{
    auto normalMatrix(glm::inverseTranspose(transform));
    auto mesh = GetMesh();
    std::set<glm::vec3, compareVec> axis;
    for (const auto& geometryItr : mesh->GetGeometries()) {
        const auto& geometry{ geometryItr.first };
        auto indices(geometry->Indices());
        if (indices != nullptr) {
            //GOOD NEWS EVERYONE ! We've got indices !
            for (auto i = 0u; i < indices->GetCount(); ++i) {
                auto index(indices->Get<unsigned>(i));//BufferHelper::Get<unsigned>(indices, i));
                auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Normal, index));
                glm::vec3 transformedNormal(normalMatrix * glm::vec4(v, 1.f));
                axis.insert(transformedNormal);
            }
            continue;
        }
        for (auto index = 0u; index < geometry->VertexCount(); ++index) {
            auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Normal, index));
            glm::vec3 transformedNormal(normalMatrix * glm::vec4(v, 1.f));
            axis.insert(transformedNormal);
        }
    }
    return axis;
}

BoundingElement::ProjectionInterval BoundingMesh::Project(const glm::vec3& axis, const glm::mat4& transform) const
{
    auto vec = axis;
    if (length(vec) < 0.0001) {
        vec = glm::vec3(1, 0, 0);
    } else
        vec = normalize(vec);
    auto mesh = GetMesh();
    auto minDot = std::numeric_limits<float>::max();
    auto maxDot = std::numeric_limits<float>::lowest();
    glm::vec3 maxV(std::numeric_limits<float>::infinity());
    glm::vec3 minV(std::numeric_limits<float>::infinity());
    for (const auto& geometryItr : mesh->GetGeometries()) {
        /*auto indices(geometry->Indices());
        if (indices != nullptr) {
            //GOOD NEWS EVERYONE ! We've got indices !
            for (auto i = 0u; i < indices->Count(); ++i) {
                auto index(BufferHelper::Get<unsigned>(indices, i));
                auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, index));
                glm::vec3 transformedVertex(transform * glm::vec4(v, 1.f));
                auto dotProd(glm::dot(transformedVertex, axis));
                if (dotProd < interval.start)
                    interval.start = dotProd;
                if (dotProd > interval.end)
                    interval.end = dotProd;
            }
            continue;
        }*/
        const auto& geometry{ geometryItr.first };
        for (auto index = 0u; index < geometry->VertexCount(); ++index) {
            auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, index));
            glm::vec3 transformedVertex(transform * glm::vec4(v, 1.f));
            auto dotProd(glm::dot(transformedVertex, vec));
            if (dotProd < minDot) {
                minDot = dotProd;
                minV = transformedVertex;
            }
            if (dotProd > maxDot) {
                maxDot = dotProd;
                maxV = transformedVertex;
            }
        }
    }
    return ProjectionInterval(vec, minV, maxV);
}

glm::mat3 BoundingMesh::LocalInertiaTensor(const float& mass) const
{
    auto maxI = glm::vec3(1.f);
    auto minI = glm::vec3(-1.f);
    auto w = fabs(maxI.x - minI.x);
    auto w2 = w * w;
    auto h = fabs(maxI.y - minI.y);
    auto h2 = h * h;
    auto d = fabs(maxI.z - minI.z);
    auto d2 = d * d;
    auto m = 1 / 12.f * mass;
    return {
        m * (h2 + d2), 0, 0,
        0, m * (w2 + d2), 0,
        0, 0, m * (w2 + h2)
    };
}
/*
glm::vec3 BoundingMesh::GetSupportPoint(const glm::vec3& axis, const glm::mat4& transform) const
{
    auto vec = axis;
    if (length(vec) < 0.0001) {
        vec = glm::vec3(1, 0, 0);
    } else
        vec = normalize(vec);
    auto mesh = GetMesh();
    glm::vec3 support(0.f);
    float maxDot(std::numeric_limits<float>::lowest());
    for (const auto& geometry : mesh->Geometrys()) {
        auto indices(geometry->Indices());
        if (indices != nullptr) {
            //GOOD NEWS EVERYONE ! We've got indices !
            for (auto i = 0u; i < indices->Count(); ++i) {
                auto index(BufferHelper::Get<unsigned>(indices, i));
                auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, index));
                v = transform * glm::vec4(v, 1.f);
                auto dotProd(glm::dot(vec, v));
                if (dotProd > maxDot) {
                    maxDot = dotProd;
                    support = v;
                }
            }
            continue;
        }
        for (auto index = 0u; index < geometry->VertexCount(); ++index) {
            auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, index));
            v = transform * glm::vec4(v, 1.f);
            auto dotProd(glm::dot(vec, v));
            if (dotProd > maxDot) {
                maxDot = dotProd;
                support = v;
            }
        }
    }
    return support;
}*/

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

std::vector<glm::vec3> BoundingMesh::Clip(glm::vec3 axis, const glm::mat4& transform) const
{
    auto mesh = GetMesh();
    std::vector<glm::vec3> output;
    for (const auto& geometryItr : mesh->GetGeometries()) {
        const auto& geometry{ geometryItr.first };
        auto indices(geometry->Indices());
        if (indices != nullptr) {
            auto startingPoint(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, indices->GetCount() - 1));
            for (auto i = 0u; i < indices->GetCount(); ++i) {
                auto index(indices->Get<unsigned>(i));//BufferHelper::Get<unsigned>(indices, i));
                auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, index));
                glm::vec3 endPoint(transform * glm::vec4(v, 1.f));
                if (IsInFront(axis, endPoint)) {
                    if (!IsInFront(axis, startingPoint))
                        output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
                    output.push_back(endPoint);
                } else if (IsInFront(axis, startingPoint))
                    output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
                startingPoint = endPoint;
            }
            continue;
        }
        auto startingPoint(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, geometry->VertexCount() - 1));
        for (auto index = 0u; index < geometry->VertexCount(); ++index) {
            auto v(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, index));
            glm::vec3 endPoint(transform * glm::vec4(v, 1.f));
            if (IsInFront(axis, endPoint)) {
                if (!IsInFront(axis, startingPoint))
                    output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
                output.push_back(endPoint);
            } else if (IsInFront(axis, startingPoint))
                output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
            startingPoint = endPoint;
        }
    }
    return output;
}
/*
BoundingElement::CollisionEdge BoundingMesh::GetBestEdge(glm::vec3 axis, const glm::mat4 &transform)
{
	auto mesh = GetMesh();
	CollisionEdge collisionEdge;
	//std::shared_ptr<::Geometry> edgeGeometry;
	//auto edgeIndex(0u);
	auto maxVDotAxis(std::numeric_limits<float>::lowest());
	auto minEDotAxis(std::numeric_limits<float>::max());
	for (const auto &geometry : mesh->Geometrys()) {
		for (auto i = 0u; i < geometry->EdgeCount(); ++i) {
			auto edge(geometry->GetEdge(i));
			auto a(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, edge[0]));
			auto b(geometry->GetVertex<glm::vec3>(Geometry::AccessorKey::Position, edge[1]));
			a = transform * glm::vec4(a, 1.f);
			b = transform * glm::vec4(b, 1.f);
			auto aDotn(dot(a, axis));
			auto bDotn(dot(b, axis));
			auto eDotn(dot(normalize(a - b), axis));
			if (aDotn >= bDotn) {
				if (aDotn == maxVDotAxis) {
					if (eDotn < minEDotAxis) {
						collisionEdge.a = a;
						collisionEdge.b = b;
						minEDotAxis = eDotn;
					}
				}
				if (aDotn > maxVDotAxis) {
					collisionEdge.a = a;
					collisionEdge.b = b;
					collisionEdge.maximumProjection = a;
					maxVDotAxis = aDotn;
					minEDotAxis = eDotn;
				}
			}
			else {
				if (bDotn == maxVDotAxis) {
					if (eDotn < minEDotAxis) {
						collisionEdge.a = a;
						collisionEdge.b = b;
						minEDotAxis = eDotn;
					}
				}
				if (bDotn > maxVDotAxis) {
					collisionEdge.a = a;
					collisionEdge.b = b;
					collisionEdge.maximumProjection = b;
					maxVDotAxis = bDotn;
					minEDotAxis = eDotn;
				}
			}
		}
	}
	return collisionEdge;
}
*/