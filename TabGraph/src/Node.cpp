/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:28
*/

#include "Node.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "Mesh/Mesh.hpp"
#include "Physics/BoundingAABB.hpp"
#include "Physics/RigidBody.hpp"
#include "Transform.hpp"

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

size_t nodeNbr = 0;

Node::Node()
    : Transform("Node_" + std::to_string(++nodeNbr))
    , _bounds(Component::Create<BoundingAABB>(glm::vec3(0), glm::vec3(1)))
{
    Engine::OnFixedUpdate().ConnectMember(this, &Node::_UpdateMeshSkin);
}

Node::Node(const Node& node)
    : Transform(node)
{
    Engine::OnFixedUpdate().ConnectMember(this, &Node::_UpdateMeshSkin);
}

Node::Node(const std::string& name)
    : Node()
{
    SetName(name);
}

Node::~Node()
{
    //_renderUpdateSlot.Disconnect();
    //Engine::OnFixedUpdate().Disconnect(_renderUpdateSlot);
}

void Node::_UpdateMeshSkin(float delta)
{
    _meshSkinUpdateDelta += delta;
    if (_meshSkinUpdateDelta < 0.015)
        return;
    _meshSkinUpdateDelta = 0;
    for (auto& mesh : GetComponents<Mesh>())
        mesh->UpdateSkin(std::static_pointer_cast<Transform>(shared_from_this()));
}

bool Node::Draw(RenderPass pass, RenderMod renderMod, bool drawChildren)
{
    bool drew = false;
    for (auto& mesh : GetComponents<Mesh>()) {
        drew |= mesh->Draw(std::static_pointer_cast<Transform>(shared_from_this()), pass, renderMod);
    }
    if (drawChildren) {
        for (auto& child : GetChildren())
            drew |= child->Draw(pass, renderMod, drawChildren);
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

#include <iostream>

void Node::AddChild(std::shared_ptr<Node> childNode)
{
    if (childNode.get() == this)
        return;
    if (HasChild(childNode)) {
        debugLog(childNode->GetName() + " is already a child of " + GetName());
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
