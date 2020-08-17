/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-17 13:45:24
*/

#include "Node.hpp"
#include "Debug.hpp"
#include "Mesh/Mesh.hpp"
#include "Physics/BoundingAABB.hpp"
#include "Physics/RigidBody.hpp"
#include "Transform.hpp"
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

Node::Node(const std::string& name)
    : Component(name)
    , _bounds(new BoundingAABB(glm::vec3(0), glm::vec3(1)))
{
    SetComponent(Transform::Create());
}

Node::~Node()
{
    //if (Parent() != nullptr)
    //    Parent()->RemoveChild(shared_from_this());
}

std::shared_ptr<Node> Node::Create(const std::string& name)
{
    //t->SetPosition(position);
    //t->SetRotation(rotation);
    //t->SetScale(scale);
    return std::shared_ptr<Node>(new Node(name));
}

bool Node::Draw(RenderMod renderMod)
{
    if (GetComponent<Mesh>() != nullptr)
        return GetComponent<Mesh>()->Draw(GetComponent<Transform>(), renderMod);
    return false;
}

bool Node::DrawDepth(RenderMod renderMod)
{
    if (GetComponent<Mesh>() != nullptr)
        return GetComponent<Mesh>()->DrawDepth(GetComponent<Transform>(), renderMod);
    return false;
}

bool Node::Drawable() const
{
    return GetComponent<Mesh>() != nullptr;
}

void Node::Load()
{
    if (GetComponent<Mesh>() != nullptr)
        GetComponent<Mesh>()->Load();
}

void Node::_FixedUpdateCPU(float)
{
    if (GetComponent<Mesh>() != nullptr)
        GetComponent<Mesh>()->UpdateSkin(GetComponent<Transform>());
}

#include <iostream>

void Node::AddChild(std::shared_ptr<Node> childNode)
{
    std::cout << __LINE__ << " " << __FUNCTION__ << " " << Name() << " " << childNode->Name() << std::endl;
    if (childNode.get() == this)
        return;
    if (HasChild(childNode)) {
        debugLog(childNode->Name() + " is already a child of " + Name());
        return;
    }
    _children.push_back(childNode);
}

void Node::RemoveChild(std::shared_ptr<Node> child)
{
    if (child == nullptr)
        return;
    auto it = std::find(_children.begin(), _children.end(), child);
    if (it != _children.end()) {
        auto child(*it);
        _children.erase(it);
        child->SetParent(nullptr);
        child->GetComponent<Transform>()->SetParent(nullptr);
    }
}

/*
** /!\ BEWARE OF THE BIG BAD LOOP !!! /!\
*/
void Node::SetParent(std::shared_ptr<Node> parent)
{
    if (parent.get() == this || Parent() == parent) {
        return;
    }
    if (Parent() != nullptr) {
        Parent()->RemoveChild(shared_from_this());
    }
    _parent = parent;
    if (parent != nullptr) {
        std::cout << __LINE__ << " " << __FUNCTION__ << " " << Name() << " " << parent->Name() << std::endl;
        parent->AddChild(shared_from_this());
    }
    std::cout << __LINE__ << " " << __FUNCTION__ << " Parent " << Parent()->Name() << " Child " << Name() << std::endl;
    if (GetComponent<Transform>()) {
        std::cout << __LINE__ << " " << __FUNCTION__ << " Parent " << Parent()->Name() << " Child " << Name() << std::endl;
        GetComponent<Transform>()->SetParent(parent ? parent->GetComponent<Transform>() : nullptr);
    }
}

std::shared_ptr<BoundingAABB> Node::GetBounds() const
{
    return _bounds;
}
