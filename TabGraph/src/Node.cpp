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
#include "Render.hpp"

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

size_t nodeNbr = 0;

Node::Node()
    : Component("Node_" + std::to_string(++nodeNbr))
    , _bounds(Component::Create<BoundingAABB>(glm::vec3(0), glm::vec3(1)))
{
    Engine::OnFixedUpdate().ConnectMember(this, &Node::_UpdateMeshSkin);
    PositionChanged.ConnectMember(this, &Node::_OnPositionChanged);
    RotationChanged.ConnectMember(this, &Node::_OnRotationChanged);
    ScaleChanged.ConnectMember(this, &Node::_OnScaleChanged);
}

Node::Node(const Node& node)
    : Component(node)
    , _Position(node._Position)
    , _Rotation(node._Rotation)
    , _Scale(node._Scale)
    , _positionChanged(node._positionChanged)
    , _rotationChanged(node._rotationChanged)
    , _scaleChanged(node._scaleChanged)
    , _localTransformMatrix(node._localTransformMatrix)
    , _localTranslationMatrix(node._localTranslationMatrix)
    , _localRotationMatrix(node._localRotationMatrix)
    , _localScaleMatrix(node._localScaleMatrix)
    , _parent(node._parent)
{
    Engine::OnFixedUpdate().ConnectMember(this, &Node::_UpdateMeshSkin);
    PositionChanged.ConnectMember(this, &Node::_OnPositionChanged);
    RotationChanged.ConnectMember(this, &Node::_OnRotationChanged);
    ScaleChanged.ConnectMember(this, &Node::_OnScaleChanged);
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
        mesh->UpdateSkin(WorldTransformMatrix());
}

bool Node::Draw(std::shared_ptr<Scene> scene, const Render::Pass &pass, const Render::Mode &renderMod, bool drawChildren)
{
    bool drew = false;
    for (auto& mesh : GetComponents<Mesh>()) {
        drew |= mesh->Draw(WorldTransformMatrix(), _prevTransformMatrix, pass, renderMod);
    }
    if (drawChildren) {
        for (auto& child : GetChildren())
            drew |= child->Draw(scene, pass, renderMod, drawChildren);
    }
    if (pass == Render::Pass::Geometry)
        _prevTransformMatrix = WorldTransformMatrix();
    return drew;
}

bool Node::DrawDepth(std::shared_ptr<Scene> scene, const Render::Mode &renderMod, bool drawChildren)
{
    bool drew = false;
    for (auto mesh : GetComponents<Mesh>()) {
        drew |= mesh->DrawDepth(WorldTransformMatrix(), renderMod);
    }
    if (drawChildren) {
        for (auto child : GetChildren())
            drew |= child->DrawDepth(scene, renderMod, drawChildren);
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
    childNode->SetParent(std::static_pointer_cast<Node>(shared_from_this()));
    //AddComponent();
}

void Node::RemoveChild(std::shared_ptr<Node> child)
{
    if (child == nullptr)
        return;
    if (HasChild(child)) {
        RemoveComponent(child);
        child->SetParent(nullptr);
        //RemoveComponent(child->GetComponent<Node>());
    }
}

#define WORLDTRANSFORMMATRIX(transform) (transform ? transform->WorldTransformMatrix() : glm::mat4(1.f))

glm::mat4 Node::WorldTransformMatrix()
{
    return WORLDTRANSFORMMATRIX(GetParent()) * GetLocalTransformMatrix();
}

glm::mat4 Node::WorldTranslationMatrix()
{
    return WORLDTRANSFORMMATRIX(GetParent()) * GetLocalTranslationMatrix();
}

glm::mat4 Node::WorldRotationMatrix()
{
    return WORLDTRANSFORMMATRIX(GetParent()) * GetLocalRotationMatrix();
}

glm::mat4 Node::WorldScaleMatrix()
{
    return WORLDTRANSFORMMATRIX(GetParent()) * GetLocalScaleMatrix();
}

inline glm::mat4 Node::GetLocalTransformMatrix()
{
    if (_positionChanged || _rotationChanged || _scaleChanged) {
        _SetLocalTransformMatrix(GetLocalTranslationMatrix() * GetLocalRotationMatrix() * GetLocalScaleMatrix());
    }
    return _localTransformMatrix;
}

inline glm::mat4 Node::GetLocalTranslationMatrix()
{
    if (_positionChanged) {
        _SetLocalTranslationMatrix(glm::translate(GetPosition()));
        _positionChanged = false;
    }
    return _localTranslationMatrix;
}

inline glm::mat4 Node::GetLocalRotationMatrix()
{
    if (_rotationChanged) {
        _SetLocalRotationMatrix(glm::mat4_cast(GetRotation()));
        _rotationChanged = false;
    }
    return _localRotationMatrix;
}

inline glm::mat4 Node::GetLocalScaleMatrix()
{
    if (_scaleChanged) {
        _SetLocalScaleMatrix(glm::scale(GetScale()));
        _scaleChanged = false;
    }
    return _localScaleMatrix;
}

glm::vec3 Node::WorldPosition() const
{
    return WORLDTRANSFORMMATRIX(GetParent()) * glm::vec4(GetPosition(), 1.f);
}

glm::quat Node::WorldRotation() const
{
    return WORLDTRANSFORMMATRIX(GetParent()) * glm::mat4_cast(GetRotation());
}

glm::vec3 Node::WorldScale() const
{
    return WORLDTRANSFORMMATRIX(GetParent()) * glm::vec4(GetScale(), 1.f);
}

/** @argument rotation : the node local rotation */
void Node::SetRotation(glm::vec3 rotation)
{
    SetRotation(glm::quat(rotation));
}

/**
 * @brief Common::Forward() * Rotation()
 * READONLY : Computed on demand
 */
glm::vec3 Node::Forward() const
{
    return GetRotation() * Common::Forward();
}

/**
 * @brief READONLY : Computed on demand
 * @return Common::Up() * Rotation()
 */
glm::vec3 Node::Up() const
{
    return GetRotation() * Common::Up();
}

/**
 * @brief READONLY : Computed on demand
 * Common::Right() * Rotation()
 */
glm::vec3 Node::Right() const
{
    return GetRotation() * Common::Right();
}

void Node::LookAt(const glm::vec3& target, const glm::vec3& up)
{
    auto direction(normalize(target - WorldPosition()));
    float directionLength = glm::length(direction);

    // Check if the direction is valid; Also deals with NaN
    if (!(directionLength > 0.0001))
        SetRotation(glm::quat(1, 0, 0, 0)); // Just return identity

    // Normalize direction
    direction /= directionLength;

    // Is the normal up (nearly) parallel to direction?
    if (glm::abs(glm::dot(direction, up)) > .9999f) {
        // Use alternative up
        SetRotation(glm::quatLookAt(direction, glm::vec3(1, 0, 0)));
    }
    else {
        SetRotation(glm::quatLookAt(direction, up));
    }
    //SetRotation(glm::quatLookAt(direction, up));
}

void Node::LookAt(const std::shared_ptr<Node>& target, const glm::vec3& up)
{
    if (target == nullptr)
        return;
    LookAt(target->WorldPosition(), up);
}

std::shared_ptr<Node> Node::GetParent() const
{
    return _parent.lock();
}

void Node::SetParent(std::shared_ptr<Node> parent)
{
    auto parentPtr{ GetParent() };
    if (parentPtr == parent || parent.get() == this)
        return;
    if (parentPtr != nullptr)
        parentPtr->RemoveChild(std::static_pointer_cast<Node>(shared_from_this()));
    _parent = parent;
    if (parent != nullptr)
        parent->AddChild(std::static_pointer_cast<Node>(shared_from_this()));
    ParentChanged(parent);
}

inline void Node::_SetLocalTransformMatrix(const glm::mat4& matrix)
{
    if (matrix != _localTransformMatrix) {
        _localTransformMatrix = matrix;
        LocalTransformMatrixChanged(matrix);
    }
}

inline void Node::_SetLocalTranslationMatrix(const glm::mat4& matrix)
{
    if (matrix != _localTranslationMatrix) {
        _localTranslationMatrix = matrix;
        LocalTranslationMatrixChanged(matrix);
    }
}

inline void Node::_SetLocalRotationMatrix(const glm::mat4& matrix)
{
    if (matrix != _localRotationMatrix) {
        _localRotationMatrix = matrix;
        LocalRotationMatrixChanged(matrix);
    }
}

inline void Node::_SetLocalScaleMatrix(const glm::mat4& matrix)
{
    if (matrix != _localScaleMatrix) {
        _localScaleMatrix = matrix;
        LocalScaleMatrixChanged(matrix);
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
    if (GetComponent<Node>()) {
        std::cout << __LINE__ << " " << __FUNCTION__ << " Parent " << Parent()->Name() << " Child " << Name() << std::endl;
        SetParent(parent ? parent->GetComponent<Node>() : nullptr);
    }
}
*/

std::shared_ptr<BoundingAABB> Node::GetBounds() const
{
    return _bounds;
}
