#pragma once

#include "Component.hpp"

class Node;
class BufferAccessor;

class MeshSkin : public Component {
public:
    static std::shared_ptr<MeshSkin> Create();
    std::shared_ptr<Node> Skeleton() const;
    void SetSkeleton(std::shared_ptr<Node> skeleton);
    std::shared_ptr<BufferAccessor> InverseBindMatrices() const;
    void SetInverseBinMatrices(std::shared_ptr<BufferAccessor> inverseBindMatrices);
    const std::vector<std::weak_ptr<Node>>& Joints() const;
    void SetJoints(std::vector<std::weak_ptr<Node>> joints);
    void AddJoint(std::shared_ptr<Node> joint);
    void RemoveJoint(std::shared_ptr<Node> joint);

protected:
    MeshSkin();

private:
    std::weak_ptr<Node> _skeleton;
    std::shared_ptr<BufferAccessor> _inverseBindMatrices;
    std::vector<std::weak_ptr<Node>> _joints;
};