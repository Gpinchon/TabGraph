/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:42
*/

#pragma once

#include "Common.hpp"
#include "Event/Signal.hpp"
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

enum class RenderPass {
    Geometry,
    Material
};

class BoundingAABB;
class RigidBody;
class Animation;
class Mesh;

class Node : public Transform {
public:
    Node();
    Node(const Node& node);
    Node(const std::string& name);
    virtual bool Draw(RenderPass pass, RenderMod = RenderMod::RenderAll, bool drawChildren = false);
    virtual bool DrawDepth(RenderMod = RenderMod::RenderAll, bool drawChildren = false);

    /** @return the Node's parent */
    //std::shared_ptr<Node> Parent() const { return _parent.lock(); };
    /** sets the parent to parent and calls AddChild in parent */
    //void SetParent(std::shared_ptr<Node> parent);
    void AddChild(std::shared_ptr<Node>);
    /** @return true if @arg child is in children list */
    bool HasChild(std::shared_ptr<Node> child) { return HasComponent(child); };
    /** @return the number of children */
    size_t ChildCount() const { return GetComponents<Node>().size(); };
    /** @return all the children */
    auto GetChildren() const { return GetComponents<Node>(); }
    /** removes parenting for specified @arg child */
    void RemoveChild(std::shared_ptr<Node> child);
    /** removes parenting for all children */
    void EmptyChildren();

    std::shared_ptr<BoundingAABB> GetBounds() const;

    virtual ~Node() /*= default*/;

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<Node>(*this);
    }
    virtual void _UpdateMeshSkin(float);
    std::shared_ptr<BoundingAABB> _bounds { nullptr };
};
