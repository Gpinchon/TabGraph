#include "Mesh/MeshSkin.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Node.hpp"

#include <algorithm>

size_t meshSkinCount(0);

MeshSkin::MeshSkin()
    : Component("MeshSkin_" + std::to_string(meshSkinCount))
{
    ++meshSkinCount;
}

const std::vector<std::shared_ptr<Node>> MeshSkin::Joints() const
{
    return _joints;
}

void MeshSkin::AddJoint(std::shared_ptr<Node> joint)
{
    _joints.push_back(joint);
}

void MeshSkin::RemoveJoint(std::shared_ptr<Node> joint)
{
    //TODO re-enable this
    //RemoveComponent(joint);
}

std::shared_ptr<BufferAccessor> MeshSkin::InverseBindMatrices() const
{
    return _inverseBindMatrices;
}

void MeshSkin::SetInverseBindMatrices(std::shared_ptr<BufferAccessor> inverseBindMatrices)
{
    _inverseBindMatrices = inverseBindMatrices;
}
