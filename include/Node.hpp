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

class Node : public Object
{
public:
    static std::shared_ptr<Node> Create(const std::string &name);
    virtual std::shared_ptr<Node> shared_from_this();
    virtual bool Draw(RenderMod = RenderMod::RenderAll) { return false; };
    virtual bool DrawDepth(RenderMod = RenderMod::RenderAll) { return false; };
    virtual bool Drawable() const { return false; };
    virtual void Load() {};
    virtual void FixedUpdate();
    virtual void Update();
    virtual void UpdateGPU();
    /** Updates all matrix and sets the final transformation matrix */
    //virtual void UpdateTransformMatrix();
    //virtual void UpdateTranslationMatrix();
    //virtual void UpdateRotationMatrix();
    //virtual void UpdateScaleMatrix();
    /** @return the basic node transformation matrix, useful for "funny" effects */
    //virtual glm::mat4 NodeTransformMatrix() const;
    /** @argument nodeTransform the node transformation matrix,  useful for "funny" effects*/
    //virtual void SetNodeTransformMatrix(glm::mat4 nodeTransform);
    //virtual glm::mat4 TransformMatrix() const;
    //virtual void SetTransformMatrix(glm::mat4);
    //virtual glm::mat4 TranslationMatrix() const;
    //virtual void SetTranslationMatrix(glm::mat4);
    //virtual glm::mat4 RotationMatrix() const;
    //virtual void SetRotationMatrix(glm::mat4);
    //virtual glm::mat4 ScaleMatrix() const;
    //virtual void SetScaleMatrix(glm::mat4);

    //virtual glm::vec3 WorldPosition() const;
    ///** @return the node local position */
    //virtual glm::vec3 Position() const;
    ///** @argument position : the node local position */
    //virtual void SetPosition(glm::vec3 position);
    //virtual glm::quat WorldRotation() const;
    ///** @return the node local rotation */
    //virtual glm::quat Rotation() const;
    ///** @argument rotation : the node local rotation */
    //virtual void SetRotation(glm::vec3 rotation);
    ///** @return the node local scale */
    //virtual void SetRotation(glm::quat rotation);
    //virtual glm::vec3 WorldScale() const;
    ///** @return the node local scale */
    //virtual glm::vec3 Scale() const;
    ///** @argument scale : the node local scale */
    //virtual void SetScale(glm::vec3 scale);
    ///** @return does the nod need transform update ? */
    //virtual bool NeedsTransformUpdate();
    ///** @argument needsTransformUpdate : new state */
    //virtual void SetNeedsTranformUpdate(bool needsTransformUpdate);
    virtual bool NeedsGPUUpdate() const;
    virtual void SetNeedsGPUUpdate(bool needsUpdate);
    //std::shared_ptr<Node> target() const;
    
    //void SetTarget(std::shared_ptr<Node>);
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

    virtual ~Node() /*= default*/;

protected:
    Node(const std::string &name);
    void AddChild(std::shared_ptr<Node>);

private:
    std::shared_ptr<BoundingAABB> _bounds { nullptr };
    std::vector<std::shared_ptr<Node>> _children;
    //std::shared_ptr<Node> _target;
    std::weak_ptr<Node> _parent;
    std::shared_ptr<::Transform> _transform;
    //glm::vec3 _position { 0, 0, 0 };
    //glm::quat _rotation { 0, 0, 0, 1 };
    //glm::vec3 _scale { 1, 1, 1 };
    //glm::vec3 _worldPosition { 0, 0, 0 };
    //glm::quat _worldRotation { 0, 0, 0, 1 };
    //glm::vec3 _worldScale { 1, 1, 1 };

    //glm::mat4 _nodeTranformationmatrix{glm::mat4(1.f)};
    //glm::mat4 _transformMatrix{glm::mat4(1.f)};
    //glm::mat4 _translationMatrix{glm::mat4(0.f)};
    //glm::mat4 _rotationMatrix{glm::mat4(0.f)};
    //glm::mat4 _scaleMatrix{glm::mat4(1.f)};
    //bool _needsTransformUpdate{true};
    bool _needsGPUUpdate{true};
};
