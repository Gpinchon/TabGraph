/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 10:31:20
*/

#pragma once

#include <memory>      // for shared_ptr, weak_ptr
#include <string>      // for string
#include <vector>      // for vector
#include "Object.hpp"  // for Object
#include "vml.h"       // for mat4_identity, mat4_zero, VEC3, MAT4

class BoundingElement;

class Node : public Object {
public:
    static std::shared_ptr<Node> create(const std::string& name, VEC3 position, VEC3 rotation, VEC3 scale);
    static std::shared_ptr<Node> get_by_name(const std::string&);
    static std::shared_ptr<Node> Get(unsigned index);
    static void add(std::shared_ptr<Node>);
    virtual std::shared_ptr<Node> shared_from_this();
    virtual void transform_update();
    virtual void FixedUpdate();
    virtual void Update();
    virtual ~Node() = default;
    MAT4& transform();
    MAT4& translate();
    MAT4& rotate();
    MAT4& scale();
    VEC3& position();
    VEC3& rotation();
    VEC3& scaling();
    VEC3& up();
    std::shared_ptr<Node> target();
    std::shared_ptr<Node> parent();
    void set_target(std::shared_ptr<Node>);
    void set_parent(std::shared_ptr<Node>);
    void add_child(std::shared_ptr<Node>);
    std::shared_ptr<Node> child(unsigned index);
    BoundingElement* bounding_element{ nullptr };

protected:
    Node(const std::string& name);
    std::vector<std::weak_ptr<Node>> _children;
    std::weak_ptr<Node> _target;
    std::weak_ptr<Node> _parent;
    VEC3 _position{ 0, 0, 0 };
    VEC3 _rotation{ 0, 0, 0 };
    VEC3 _scaling{ 1, 1, 1 };
    VEC3 _up{ 0, 1, 0 };
    MAT4 _transform{ mat4_identity() };
    MAT4 _translate{ mat4_zero() };
    MAT4 _rotate{ mat4_zero() };
    MAT4 _scale{ mat4_identity() };

private:
    static std::vector<std::shared_ptr<Node>> _nodes;
};
