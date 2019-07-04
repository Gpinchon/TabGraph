/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-28 13:16:58
*/

#pragma once

#include "Object.hpp" // for Object
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

//#include "glm"       // for glm::identity, mat4_zero, glm::vec3, glm::mat4

class BoundingElement;

class Node : public Object {
public:
    static std::shared_ptr<Node> create(const std::string& name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    static std::shared_ptr<Node> get_by_name(const std::string&);
    static std::shared_ptr<Node> Get(unsigned index);
    static void add(std::shared_ptr<Node>);
    virtual std::shared_ptr<Node> shared_from_this();
    virtual void transform_update();
    virtual void FixedUpdate();
    virtual void Update();
    virtual ~Node() = default;
    glm::mat4& transform();
    glm::mat4& translate();
    glm::mat4& rotate();
    glm::mat4& scale();
    glm::vec3& position();
    glm::vec3& rotation();
    glm::vec3& scaling();
    glm::vec3& up();
    std::shared_ptr<Node> target();
    std::shared_ptr<Node> parent();
    void set_target(std::shared_ptr<Node>);
    void set_parent(std::shared_ptr<Node>);
    void add_child(std::shared_ptr<Node>);
    std::shared_ptr<Node> child(unsigned index);
    BoundingElement* bounding_element { nullptr };

protected:
    Node(const std::string& name);
    std::vector<std::weak_ptr<Node>> _children;
    std::weak_ptr<Node> _target;
    std::weak_ptr<Node> _parent;
    glm::vec3 _position { 0, 0, 0 };
    glm::vec3 _rotation { 0, 0, 0 };
    glm::vec3 _scaling { 1, 1, 1 };
    glm::vec3 _up { 0, 1, 0 };
    glm::mat4 _transform { glm::mat4(1.f) };
    glm::mat4 _translate { glm::mat4(0.f) };
    glm::mat4 _rotate { glm::mat4(0.f) };
    glm::mat4 _scale { glm::mat4(1.f) };

private:
    static std::vector<std::shared_ptr<Node>> _nodes;
};
