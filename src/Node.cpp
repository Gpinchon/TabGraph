/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-17 20:55:06
*/

#include "Physics/RigidBody.hpp"
#include "Physics/BoundingAABB.hpp"
#include "Debug.hpp"
#include "Mesh/Mesh.hpp"
#include "Node.hpp"
#include "Transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext.hpp>
#include <iostream>

Node::Node(const std::string &name)
    : Object(name), _bounds(new BoundingAABB(glm::vec3(0), glm::vec3(1))), _transform(Transform::Create(name + "_transform"))
{
}

Node::~Node() {
    //if (Parent() != nullptr)
    //    Parent()->RemoveChild(shared_from_this());
}

std::shared_ptr<Node> Node::Create(const std::string &name)
{
    //t->SetPosition(position);
    //t->SetRotation(rotation);
    //t->SetScale(scale);
    return std::shared_ptr<Node>(new Node(name));
}

std::shared_ptr<Node> Node::shared_from_this()
{
    return (std::static_pointer_cast<Node>(Object::shared_from_this()));
}

bool Node::Draw(RenderMod renderMod)
{
    if (GetMesh() != nullptr)
        return GetMesh()->Draw(GetTransform(), renderMod);
    return false;
}

bool Node::DrawDepth(RenderMod renderMod)
{
    if (GetMesh() != nullptr)
        return GetMesh()->DrawDepth(GetTransform(), renderMod);
    return false;
}

bool Node::Drawable() const
{
    return GetMesh() != nullptr;
}

void Node::Load()
{
    if (GetMesh() != nullptr)
        GetMesh()->Load();
}

void Node::FixedUpdate()
{
    if (GetMesh() != nullptr)
        GetMesh()->UpdateSkin(GetTransform());
}

void Node::Update()
{
}

void Node::UpdateGPU()
{
    //TODO GET REAL DELTA
    if (GetMesh() != nullptr)
        GetMesh()->UpdateGPU(0.f);
}

void Node::AddChild(std::shared_ptr<Node> childNode)
{
    if (childNode == shared_from_this())
        return;
    if (HasChild(childNode)) {
        debugLog(childNode->Name() + " is already a child of " + Name());
        return;
    }
    _children.push_back(childNode);
    childNode->SetParent(shared_from_this());
}

void Node::RemoveChild(std::shared_ptr<Node> child)
{
    if (child == nullptr)
        return;
    auto it = std::find(_children.begin(), _children.end(), child);
    if(it != _children.end()) {
        auto child(*it);
        _children.erase(it);
        child->SetParent(nullptr);
        child->GetTransform()->SetParent(nullptr);
    }
    //_children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
}

/*
** /!\ BEWARE OF THE BIG BAD LOOP !!! /!\
*/
void Node::SetParent(std::shared_ptr<Node> parent)
{
    if (parent == shared_from_this() || _parent.lock() == parent)
    {
        return;
    }
    if (Parent() != nullptr) {
        Parent()->RemoveChild(shared_from_this());
    }
    _parent = parent;
    if (parent != nullptr) {
        parent->AddChild(shared_from_this());
    }
    if (GetTransform())
        GetTransform()->SetParent(parent ? parent->GetTransform() : nullptr);
}

bool Node::NeedsGPUUpdate() const
{
    return _needsGPUUpdate;
}

void Node::SetNeedsGPUUpdate(bool needsUpdate)
{
    _needsGPUUpdate = needsUpdate;
}

std::shared_ptr<BoundingAABB> Node::GetBounds() const
{
    return _bounds;
}

void Node::SetMesh(const std::shared_ptr<Mesh> &mesh)
{
    _mesh = mesh;
}

std::shared_ptr<Mesh> Node::GetMesh() const
{
    return _mesh;
}
