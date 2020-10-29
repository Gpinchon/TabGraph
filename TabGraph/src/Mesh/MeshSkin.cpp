#include "Mesh/MeshSkin.hpp"
#include <algorithm>

MeshSkin::MeshSkin()
    : Component()
{
    static uint64_t meshSkinCount(0);
    ++meshSkinCount;
    SetName("MeshSkin_" + std::to_string(meshSkinCount));
}

std::shared_ptr<MeshSkin> MeshSkin::Create()
{
    return tools::make_shared<MeshSkin>();
}

std::shared_ptr<Node> MeshSkin::Skeleton() const
{
    return _skeleton.lock();
}

void MeshSkin::SetSkeleton(std::shared_ptr<Node> skeleton)
{
    _skeleton = skeleton;
}

const std::vector<std::weak_ptr<Node>>& MeshSkin::Joints() const
{
    return _joints;
}

void MeshSkin::SetJoints(std::vector<std::weak_ptr<Node>> joints)
{
    _joints = joints;
}

void MeshSkin::AddJoint(std::shared_ptr<Node> joint)
{
    _joints.push_back(joint);
}

void MeshSkin::RemoveJoint(std::shared_ptr<Node> joint)
{
    _joints.erase(std::remove_if(_joints.begin(), _joints.end(), [joint](auto it) { return it.lock() == joint; }), _joints.end());
}

std::shared_ptr<BufferAccessor> MeshSkin::InverseBindMatrices() const
{
    return _inverseBindMatrices;
}

void MeshSkin::SetInverseBinMatrices(std::shared_ptr<BufferAccessor> inverseBindMatrices)
{
    _inverseBindMatrices = inverseBindMatrices;
}
