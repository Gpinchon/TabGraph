/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-23 13:57:23
*/

#include "Node.hpp"
#include "Debug.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext.hpp>
#include <algorithm>
#include <iostream>

Node::Node(const std::string &name)
    : Object(name)
{
}

Node::~Node() {
    if (Parent() != nullptr)
        Parent()->RemoveChild(shared_from_this());
}

std::shared_ptr<Node> Node::Create(const std::string &name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    auto t = std::shared_ptr<Node>(new Node(name));
    t->SetPosition(position);
    t->SetRotation(rotation);
    t->SetScale(scale);
    return (t);
}

std::shared_ptr<Node> Node::shared_from_this()
{
    return (std::static_pointer_cast<Node>(Object::shared_from_this()));
}

void Node::FixedUpdate()
{
}

void Node::Update()
{
}

void Node::UpdateGPU()
{
}

void Node::UpdateTransformMatrix()
{
    if (!NeedsTransformUpdate())
        return;
    UpdateTranslationMatrix();
    UpdateRotationMatrix();
    UpdateScaleMatrix();
    SetTransformMatrix(TranslationMatrix() * RotationMatrix() * ScaleMatrix());
    if (auto parentPtr = Parent(); parentPtr != nullptr) 
        SetTransformMatrix(parentPtr->TransformMatrix() * TransformMatrix());
    SetNeedsTranformUpdate(false);
    for (auto child : _children)
        child->SetNeedsTranformUpdate(true);
    //if (NeedsTransformUpdate()) {
    //    UpdateTranslationMatrix();
    //    UpdateRotationMatrix();
    //    UpdateScaleMatrix();
    //}
    //SetTransformMatrix(TranslationMatrix() * RotationMatrix() * ScaleMatrix());
    //if (auto parentPtr = Parent(); parentPtr != nullptr) 
    //    SetTransformMatrix(parentPtr->TransformMatrix() * TransformMatrix());
    //for (auto child : _children)
    //    child->SetNeedsTranformUpdate(true);
    //SetNeedsTranformUpdate(false);
}

void Node::UpdateTranslationMatrix()
{
    SetTranslationMatrix(glm::translate(Position()));
}

void Node::UpdateRotationMatrix()
{
    SetRotationMatrix(glm::mat4_cast(Rotation()));
}

void Node::UpdateScaleMatrix()
{
    SetScaleMatrix(glm::scale(Scale()));
}

std::vector<std::shared_ptr<Node>> Node::Children() const
{
    return _children;
}

void Node::AddChild(std::shared_ptr<Node> childNode)
{
    if (childNode == shared_from_this())
    {
        return;
    }
    if (std::find(_children.begin(), _children.end(), childNode) != _children.end()) {
        debugLog(childNode->Name() + " is already a child of " + Name());
        return;
    }
    _children.push_back(childNode);
    childNode->SetParent(shared_from_this());
}

void Node::RemoveChild(std::shared_ptr<Node> child)
{
    //auto it = std::find(_children.begin(), _children.end(), child);
    //if(it != _children.end()) {
    //    auto child(*it);
    //    _children.erase(it);
    //    if (child != nullptr)
    //        child->SetParent(nullptr);
    //}
    _children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
}

std::shared_ptr<Node> Node::target() const
{
    return (_target);
}

void Node::SetTarget(std::shared_ptr<Node> tgt)
{
    _target = tgt;
}

std::shared_ptr<Node> Node::Parent() const
{
    return (_parent.lock());
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
    if (Parent() != nullptr)
        Parent()->RemoveChild(shared_from_this());
    _parent = parent;
    if (parent != nullptr)
        parent->AddChild(shared_from_this());
}

glm::vec3 Node::Position() const
{
    return _position;
}

void Node::SetPosition(glm::vec3 position)
{
    _position = position;
    SetNeedsTranformUpdate(true);
}

glm::quat Node::Rotation() const
{
    return (_rotation);
}

void Node::SetRotation(glm::vec3 rotation)
{
    /*auto angles(glm::eulerAngles(rotation));
    angles = glm::degrees(glm::vec3(angles.x, angles.y, angles.z));*/
    SetRotation(glm::quat(rotation));
}

void Node::SetRotation(glm::quat rotation)
{
    _rotation = rotation;
    SetNeedsTranformUpdate(true);
}

glm::vec3 Node::Scale() const
{
    return (_scale);
}

void Node::SetScale(glm::vec3 scale)
{
    _scale = scale;
    SetNeedsTranformUpdate(true);
}

/*glm::mat4 Node::NodeTransformMatrix() const
{
    return _nodeTranformationmatrix;
}*/

#include <glm/gtx/matrix_decompose.hpp>

void Node::SetNodeTransformMatrix(glm::mat4 nodeTransform)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(nodeTransform, scale, rotation, translation, skew, perspective);
    SetPosition(translation);
    SetRotation(glm::normalize(rotation));
    SetScale(scale);
    //_nodeTranformationmatrix = nodeTransform;
}

glm::mat4 Node::TransformMatrix() const
{
    return (_transformMatrix);
}

void Node::SetTransformMatrix(glm::mat4 transform)
{
    _transformMatrix = transform;
}

glm::mat4 Node::TranslationMatrix() const
{
    return (_translationMatrix);
}

void Node::SetTranslationMatrix(glm::mat4 translation)
{
    _translationMatrix = translation;
}

glm::mat4 Node::RotationMatrix() const
{
    return (_rotationMatrix);
}

void Node::SetRotationMatrix(glm::mat4 rotation)
{
    _rotationMatrix = rotation;
}

glm::mat4 Node::ScaleMatrix() const
{
    return (_scaleMatrix);
}

void Node::SetScaleMatrix(glm::mat4 scale)
{
    _scaleMatrix = scale;
}

bool Node::NeedsTransformUpdate()
{
    return _needsTransformUpdate;
}

void Node::SetNeedsTranformUpdate(bool needsTransformUpdate)
{
    _needsTransformUpdate = needsTransformUpdate;
}

bool Node::NeedsGPUUpdate() const
{
    return _needsGPUUpdate;
}

void Node::SetNeedsGPUUpdate(bool needsUpdate)
{
    _needsGPUUpdate = needsUpdate;
}
