#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Object.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Shape/Mesh.hpp>
#include <SG/Buffer/Accessor.hpp>

#include <vector>
#include <glm/mat4x4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Node;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Skin : public Inherit<Object, Skin> {
    using Joints = std::vector<ECS::DefaultRegistry::EntityRefType>;
    using MatrixAccessor = TypedBufferAccessor<glm::mat4>;
    PROPERTY(MatrixAccessor, InverseBindMatrices, );
    PROPERTY(Joints, Joints, );

public:
    Skin() = default;
    void AddJoint(const ECS::DefaultRegistry::EntityRefType& joint) {
        GetJoints().push_back(joint);
    }
    void RemoveJoint(const ECS::DefaultRegistry::EntityRefType joint) {
        GetJoints().erase(std::remove(GetJoints().begin(), GetJoints().end(), joint), GetJoints().end());
    }
};
}