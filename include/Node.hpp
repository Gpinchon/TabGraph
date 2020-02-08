/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-09 17:47:42
*/

#pragma once

#include "Object.hpp" // for Object
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string
#include <vector> // for vector

class BoundingElement;

class Node : public Object
{
public:
    static std::shared_ptr<Node> Create(const std::string &name, glm::vec3 position = glm::vec3(), glm::vec3 rotation = glm::vec3(), glm::vec3 scale = glm::vec3(1));
    static std::shared_ptr<Node> GetByName(const std::string &);
    static std::shared_ptr<Node> Get(unsigned index);
    /** Adds the Node to the Node list */
    static void Add(std::shared_ptr<Node>);
    virtual std::shared_ptr<Node> shared_from_this();
    virtual void FixedUpdate();
    virtual void Update();
    /** Updates all matrix and sets the final transformation matrix */
    virtual void UpdateTransformMatrix();
    virtual void UpdateTranslationMatrix();
    virtual void UpdateRotationMatrix();
    virtual void UpdateScaleMatrix();
    /** @return the basic node transformation matrix, useful for "funny" effects */
    virtual glm::mat4 NodeTransformMatrix() const;
    /** @argument nodeTransform the node transformation matrix,  useful for "funny" effects*/
    virtual void SetNodeTransformMatrix(glm::mat4 nodeTransform);
    virtual glm::mat4 TransformMatrix() const;
    virtual void SetTransformMatrix(glm::mat4);
    virtual glm::mat4 TranslationMatrix() const;
    virtual void SetTranslationMatrix(glm::mat4);
    virtual glm::mat4 RotationMatrix() const;
    virtual void SetRotationMatrix(glm::mat4);
    virtual glm::mat4 ScaleMatrix() const;
    virtual void SetScaleMatrix(glm::mat4);
    /** @return the node local position */
    virtual glm::vec3 Position() const;
    /** @argument position : the node local position */
    virtual void SetPosition(glm::vec3 position);
    /** @return the node local rotation */
    virtual glm::vec3 Rotation() const;
    /** @argument rotation : the node local rotation */
    virtual void SetRotation(glm::vec3 rotation);
    /** @return the node local scale */
    virtual glm::vec3 Scale() const;
    /** @argument scale : the node local scale */
    virtual void SetScale(glm::vec3 scale);
    std::shared_ptr<Node> target();
    std::shared_ptr<Node> parent();
    void set_target(std::shared_ptr<Node>);
    void set_parent(std::shared_ptr<Node>);
    void add_child(std::shared_ptr<Node>);
    std::vector<std::shared_ptr<Node>> Children() const;
    BoundingElement *bounding_element{nullptr};
    virtual ~Node() /*= default*/;

protected:
    Node(const std::string &name);

private:
    static std::vector<std::shared_ptr<Node>> _nodes;
    std::vector<std::shared_ptr<Node>> _children;
    std::shared_ptr<Node> _target;
    std::shared_ptr<Node> _parent;
    glm::vec3 _position{0, 0, 0};
    glm::vec3 _rotation{0, 0, 0};
    glm::vec3 _scale{1, 1, 1};
    glm::mat4 _nodeTranformationmatrix{glm::mat4(1.f)};
    glm::mat4 _transformMatrix{glm::mat4(1.f)};
    glm::mat4 _translationMatrix{glm::mat4(0.f)};
    glm::mat4 _rotationMatrix{glm::mat4(0.f)};
    glm::mat4 _scaleMatrix{glm::mat4(1.f)};
};
