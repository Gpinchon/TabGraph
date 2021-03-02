#pragma once

#include "Component.hpp"

class Node;
class BufferAccessor;

class MeshSkin : public Component {
public:
    MeshSkin();
    std::shared_ptr<BufferAccessor> InverseBindMatrices() const;
    void SetInverseBindMatrices(std::shared_ptr<BufferAccessor> inverseBindMatrices);
    const std::vector<std::shared_ptr<Node>> Joints() const;
    void AddJoint(std::shared_ptr<Node> joint);
    void RemoveJoint(std::shared_ptr<Node> joint);

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<MeshSkin>(*this);
    }
    virtual void _Replace(const std::shared_ptr<Component> oldComponent, const std::shared_ptr<Component> newComponent) override {
        auto nodePtr = std::dynamic_pointer_cast<Node>(oldComponent);
        if (nodePtr != nullptr)
            std::replace(_joints.begin(), _joints.end(), nodePtr, std::static_pointer_cast<Node>(newComponent));
    }
    //std::weak_ptr<Node> _skeleton;
    std::vector<std::shared_ptr<Node>> _joints;
    std::shared_ptr<BufferAccessor> _inverseBindMatrices;
};