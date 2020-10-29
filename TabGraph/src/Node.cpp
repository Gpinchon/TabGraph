/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-20 17:03:04
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
    : Transform(name)
    , _bounds(tools::make_shared<BoundingAABB>(glm::vec3(0), glm::vec3(1)))
{
}

Node::~Node()
{
}

std::shared_ptr<Node> Node::Create(const std::string& name)
{
    auto node(tools::make_shared<Node>(name));
    return node;
}

bool Node::Draw(RenderMod renderMod, bool drawChildren)
{
    bool drew = false;
    for (auto mesh : GetComponents<Mesh>()) {
        drew |= mesh->Draw(std::static_pointer_cast<Transform>(shared_from_this()), renderMod);
    }
    if (drawChildren) {
        for (auto child : GetChildren())
            drew |= child->Draw(renderMod, drawChildren);
    }
    return drew;
}

bool Node::DrawDepth(RenderMod renderMod, bool drawChildren)
{
    bool drew = false;
    for (auto mesh : GetComponents<Mesh>()) {
        drew |= mesh->DrawDepth(std::static_pointer_cast<Transform>(shared_from_this()), renderMod);
    }
    if (drawChildren) {
        for (auto child : GetChildren())
            drew |= child->DrawDepth(renderMod, drawChildren);
    }
    return drew;
}

void Node::_FixedUpdateCPU(float)
{
    if (GetComponent<Mesh>() != nullptr)
        GetComponent<Mesh>()->UpdateSkin(std::static_pointer_cast<Transform>(shared_from_this()));
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
    AddComponent(childNode);
    childNode->SetParent(std::static_pointer_cast<Transform>(shared_from_this()));
    //AddComponent();
}

void Node::RemoveChild(std::shared_ptr<Node> child)
{
    if (child == nullptr)
        return;
    if (HasChild(child)) {
        RemoveComponent(child);
        child->SetParent(nullptr);
        //RemoveComponent(child->GetComponent<Transform>());
    }
}

/*
** /!\ BEWARE OF THE BIG BAD LOOP !!! /!\
*/
/*
void Node::SetParent(std::shared_ptr<Node> parent)
{
    if (parent.get() == this || Parent() == parent) {
        return;
    }
    if (Parent() != nullptr) {
        Parent()->RemoveComponent(std::static_pointer_cast<Node>(shared_from_this()));
    }
    _parent = parent;
    if (parent != nullptr) {
        std::cout << __LINE__ << " " << __FUNCTION__ << " " << Name() << " " << parent->Name() << std::endl;
        parent->AddChild(std::static_pointer_cast<Node>(shared_from_this()));
    }
    std::cout << __LINE__ << " " << __FUNCTION__ << " Parent " << Parent()->Name() << " Child " << Name() << std::endl;
    if (GetComponent<Transform>()) {
        std::cout << __LINE__ << " " << __FUNCTION__ << " Parent " << Parent()->Name() << " Child " << Name() << std::endl;
        SetParent(parent ? parent->GetComponent<Transform>() : nullptr);
    }
}
*/

std::shared_ptr<BoundingAABB> Node::GetBounds() const
{
    return _bounds;
}
