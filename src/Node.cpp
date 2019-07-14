/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-07-14 22:49:53
*/

#include "Node.hpp"
#include <glm/ext.hpp>

std::vector<std::shared_ptr<Node>> Node::_nodes;

Node::Node(const std::string& name)
    : Object(name)
{
}

std::shared_ptr<Node> Node::create(const std::string& name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    auto t = std::shared_ptr<Node>(new Node(name));
    t->_position = position;
    t->_rotation = rotation;
    t->_scaling = scale;
    //t->_transform = new_transform(position, rotation, scale, up());
    t->Update();
    add(t);
    return (t);
}

std::shared_ptr<Node> Node::get_by_name(const std::string& name)
{
    for (auto n : _nodes) {
        if (name == n->name())
            return (n);
    }
    return (nullptr);
}

std::shared_ptr<Node> Node::Get(unsigned index)
{
    if (index >= _nodes.size())
        return (nullptr);
    return (_nodes.at(index));
}

std::shared_ptr<Node> Node::shared_from_this()
{
    return (std::static_pointer_cast<Node>(Object::shared_from_this()));
}

void Node::add(std::shared_ptr<Node> node)
{
    _nodes.push_back(node);
}

void Node::transform_update()
{
    _translate = glm::translate(glm::mat4(1.f), _position);
    _rotate = glm::rotate(glm::mat4(1.f), _rotation.x, glm::vec3(1, 0, 0));
    _rotate = glm::rotate(_rotate, _rotation.y, glm::vec3(0, 1, 0));
    _rotate = glm::rotate(_rotate, _rotation.z, glm::vec3(0, 0, 1));
    _scale = glm::scale(glm::mat4(1.f), _scaling);
    _transform = _translate * _rotate * _scale;
    auto parentPtr = parent();
    if (parentPtr != nullptr) {
        parentPtr->transform_update();
        _transform = parentPtr->TransformMatrix() * _transform;
    }
}

void Node::FixedUpdate()
{
}

void Node::Update()
{
}

void Node::add_child(std::shared_ptr<Node> childNode)
{
    if (childNode == shared_from_this()) {
        return;
    }
    _children.push_back(childNode);
    childNode->set_parent(shared_from_this());
}

std::shared_ptr<Node> Node::target()
{
    return (_target.lock());
}

void Node::set_target(std::shared_ptr<Node> tgt)
{
    _target = tgt;
}

std::shared_ptr<Node> Node::parent()
{
    return (_parent.lock());
}

/*
** /!\ BEWARE OF THE BIG BAD LOOP !!! /!\
*/
void Node::set_parent(std::shared_ptr<Node> prt)
{
    if (prt == shared_from_this() || _parent.lock() == prt) {
        return;
    }
    _parent = prt;
    prt->add_child(shared_from_this());
}

glm::vec3 Node::Position() const
{
    return _position;
}

void Node::SetPosition(glm::vec3 position)
{
    _position = position;
}

glm::vec3 Node::Rotation() const
{
    return (_rotation);
}

glm::vec3 Node::Scale() const
{
    return (_scaling);
}

glm::mat4 Node::TransformMatrix() const
{
    return (_transform);
}

glm::mat4 Node::TranslationMatrix() const
{
    return (_translate);
}

glm::mat4 Node::RotationMatrix() const
{
    return (_rotate);
}

glm::mat4 Node::ScaleMatrix() const
{
    return (_scale);
}
