#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Object.hpp>
#include <Core/Inherit.hpp>
#include <Shapes/Mesh/Mesh.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Nodes {
class Node;
}
namespace Buffer {
class Accessor;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Mesh::Skin : public Core::Inherit<Core::Object, Mesh::Skin> {
public:
    Skin();
    std::shared_ptr<Buffer::Accessor> InverseBindMatrices() const
    {
        return _inverseBindMatrices;
    }
    void SetInverseBindMatrices(std::shared_ptr<Buffer::Accessor> inverseBindMatrices)
    {
        _inverseBindMatrices = inverseBindMatrices;
    }
    auto& Joints() const
    {
        return _joints;
    }
    void AddJoint(std::shared_ptr<Nodes::Node> joint)
    {
        _joints.push_back(joint);
    }
    void RemoveJoint(std::shared_ptr<Nodes::Node> joint)
    {
        std::remove(_joints.begin(), _joints.end(), joint);
    }

private:
    std::vector<std::shared_ptr<Nodes::Node>> _joints;
    std::shared_ptr<Buffer::Accessor> _inverseBindMatrices;
};
}
