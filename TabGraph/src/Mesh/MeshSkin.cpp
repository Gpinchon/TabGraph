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
    //return GetComponents<Node>();
}

void MeshSkin::AddJoint(std::shared_ptr<Node> joint)
{
    _joints.push_back(joint);
    //AddComponent(joint);
}

void MeshSkin::RemoveJoint(std::shared_ptr<Node> joint)
{
    //RemoveComponent(joint);
}

std::shared_ptr<BufferAccessor> MeshSkin::InverseBindMatrices() const
{
    return GetComponent<BufferAccessor>();
}

void MeshSkin::SetInverseBindMatrices(std::shared_ptr<BufferAccessor> inverseBindMatrices)
{
    SetComponent<BufferAccessor>(inverseBindMatrices);
}
