/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 20:57:57
*/

#pragma once

#include "Common.hpp"
#include "Component.hpp" // for Component
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
class Transform;
class Animation;
class Mesh;

class Node : public Component, public std::enable_shared_from_this<Node> {
public:
    static std::shared_ptr<Node> Create(const std::string& name);
    virtual bool Draw(RenderMod = RenderMod::RenderAll);
    virtual bool DrawDepth(RenderMod = RenderMod::RenderAll);
    virtual bool Drawable() const;
    virtual void Load();
    //virtual void FixedUpdate();

    /** @return the Node's parent */
    std::shared_ptr<Node> Parent() const { return _parent.lock(); };
    /** sets the parent to parent and calls AddChild in parent */
    void SetParent(std::shared_ptr<Node> parent);
    /** @return true if @arg child is in children list */
    bool HasChild(std::shared_ptr<Node> child)
    {
        auto children = GetComponents<Node>();
        return std::find(children.begin(), children.end(), child) != children.end();
    };
    /** @return the number of children */
    size_t ChildCount() const { return GetComponents<Node>().size(); };
    /** @return the child at @arg index */
    std::shared_ptr<Node> GetChild(const size_t index) const { return GetComponents<Node>().at(index); };
    /** removes parenting for specified @arg child */
    void RemoveChild(std::shared_ptr<Node> child);
    /** removes parenting for all children */
    void EmptyChildren();

    std::shared_ptr<BoundingAABB> GetBounds() const;

    virtual ~Node() /*= default*/;

protected:
    Node(const std::string& name);
    void AddChild(std::shared_ptr<Node>);

private:
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override;
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
    std::shared_ptr<BoundingAABB> _bounds { nullptr };
    std::weak_ptr<Node> _parent;
};
