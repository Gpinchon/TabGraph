/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-09 17:47:42
*/

#pragma once

#include "Common.hpp"
#include "Object.hpp" // for Object
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector
#include <algorithm>

enum class RenderMod
{
    RenderAll,
    RenderOpaque,
    RenderTransparent
};

class BoundingAABB;
class RigidBody;
class Transform;
class Animation;
class Mesh;

class Node : public Object
{
public:
    static std::shared_ptr<Node> Create(const std::string &name);
    virtual std::shared_ptr<Node> shared_from_this();
    virtual bool Draw(RenderMod = RenderMod::RenderAll);
    virtual bool DrawDepth(RenderMod = RenderMod::RenderAll);
    virtual bool Drawable() const;
    virtual void Load();
    virtual void FixedUpdate();
    virtual void Update();
    virtual void UpdateGPU();
    virtual bool NeedsGPUUpdate() const;
    virtual void SetNeedsGPUUpdate(bool needsUpdate);
    
    /** @return the Node's parent */
    std::shared_ptr<Node> Parent() const { return _parent.lock(); };
    /** sets the parent to parent and calls AddChild in parent */
    void SetParent(std::shared_ptr<Node> parent);
    /** @return true if @arg child is in children list */
    bool HasChild(std::shared_ptr<Node> child) { return std::find(_children.begin(), _children.end(), child) != _children.end(); };
    /** @return the number of children */
    size_t ChildCount() const { return _children.size(); };
    /** @return the child at @arg index */
    std::shared_ptr<Node> GetChild(const size_t index) const { return _children.at(index); };
    /** removes parenting for specified @arg child */
    void RemoveChild(std::shared_ptr<Node> child);
    /** removes parenting for all children */
    void EmptyChildren();
    //std::vector<std::shared_ptr<Node>> Children() const;

    std::shared_ptr<Transform> GetTransform() const { return _transform; }
    void SetTransform(const std::shared_ptr<::Transform> &transform) { _transform = transform; };

    std::shared_ptr<BoundingAABB> GetBounds() const;

    ///** @arg animation : an animation to add to the node */
    //void AddAnimation(const std::shared_ptr<Animation> &animation);
    ///** @arg animation : the animation to remove from the node */
    //void RemoveAnimation(const std::shared_ptr<Animation> &animation);
    ///** @return the total number of animation */
    //size_t GetAnimationCount() const;
    ///** @return the animation at the specified index (subject to change) */
    //std::shared_ptr<Animation> GetAnimation(size_t index);
    ///** @return the animation with the specified name */
    //std::shared_ptr<Animation> GetAnimation(const std::string &name);

    void SetMesh(const std::shared_ptr<Mesh> &mesh);
    std::shared_ptr<Mesh> GetMesh() const;

    virtual ~Node() /*= default*/;

protected:
    Node(const std::string &name);
    void AddChild(std::shared_ptr<Node>);

private:
    std::shared_ptr<BoundingAABB> _bounds { nullptr };
    std::vector<std::shared_ptr<Node>> _children;
    std::shared_ptr<Mesh> _mesh { nullptr };
    std::weak_ptr<Node> _parent;
    std::shared_ptr<::Transform> _transform;
    bool _needsGPUUpdate{true};
};
