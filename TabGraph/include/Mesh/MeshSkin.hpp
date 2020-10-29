#pragma once

#include "Component.hpp"

class Node;
class BufferAccessor;

class MeshSkin : public Component {
public:
    MeshSkin();
    static std::shared_ptr<MeshSkin> Create();
    std::shared_ptr<Node> Skeleton() const;
    void SetSkeleton(std::shared_ptr<Node> skeleton);
    std::shared_ptr<BufferAccessor> InverseBindMatrices() const;
    void SetInverseBinMatrices(std::shared_ptr<BufferAccessor> inverseBindMatrices);
    const std::vector<std::weak_ptr<Node>>& Joints() const;
    void SetJoints(std::vector<std::weak_ptr<Node>> joints);
    void AddJoint(std::shared_ptr<Node> joint);
    void RemoveJoint(std::shared_ptr<Node> joint);

private:
    virtual std::shared_ptr<Component> _Clone() const override {
        return tools::make_shared<MeshSkin>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float) override {};
    virtual void _UpdateGPU(float) override {};
    virtual void _FixedUpdateCPU(float) override {};
    virtual void _FixedUpdateGPU(float) override {};
    std::weak_ptr<Node> _skeleton;
    std::shared_ptr<BufferAccessor> _inverseBindMatrices;
    std::vector<std::weak_ptr<Node>> _joints;
};