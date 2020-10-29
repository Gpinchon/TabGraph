/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-20 14:49:45
*/

#pragma once

#include "Common.hpp"
#include "Transform.hpp" // for Transform
#include <algorithm>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

enum class RenderMod {
    RenderAll,
    RenderOpaque,
    RenderTransparent
};

class BoundingAABB;
class RigidBody;
class Animation;
class Mesh;

class Node : public Transform {
public:
    Node(const std::string& name);
    static std::shared_ptr<Node> Create(const std::string& name);
    virtual bool Draw(RenderMod = RenderMod::RenderAll, bool drawChildren = false);
    virtual bool DrawDepth(RenderMod = RenderMod::RenderAll, bool drawChildren = false);

    /** @return the Node's parent */
    //std::shared_ptr<Node> Parent() const { return _parent.lock(); };
    /** sets the parent to parent and calls AddChild in parent */
    //void SetParent(std::shared_ptr<Node> parent);
    void AddChild(std::shared_ptr<Node>);
    /** @return true if @arg child is in children list */
    bool HasChild(std::shared_ptr<Node> child) { return HasComponentOfType(child); };
    /** @return the number of children */
    size_t ChildCount() const { return GetComponents<Node>().size(); };
    /** @return the child at @arg index */
    std::shared_ptr<Node> GetChild(const size_t index) const { return GetComponents<Node>().at(index); };
    /** @return all the children */
    std::vector <std::shared_ptr<Node>> GetChildren() const { return GetComponents<Node>(); }
    /** removes parenting for specified @arg child */
    void RemoveChild(std::shared_ptr<Node> child);
    /** removes parenting for all children */
    void EmptyChildren();

    std::shared_ptr<BoundingAABB> GetBounds() const;

    virtual ~Node() /*= default*/;

private:
    virtual std::shared_ptr<Component> _Clone() const override {
        return tools::make_shared<Node>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override;
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
    std::shared_ptr<BoundingAABB> _bounds { nullptr };
};
