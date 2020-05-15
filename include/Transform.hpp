#pragma once
#include "Common.hpp"
#include "Object.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

/** Header-only class for performance */
class Transform : public Object
{
public:
    static std::shared_ptr<Transform> Create(const std::string &name) { return std::shared_ptr<Transform>(new Transform(name));  };
    std::shared_ptr<Transform> shared_from_this() { return std::static_pointer_cast<Transform>(Object::shared_from_this()); };
    inline glm::mat4 WorldTransformMatrix() const {
        auto worldTransformMatrix(Parent() ? Parent()->WorldTransformMatrix() * LocalTransformMatrix() : LocalTransformMatrix());
        //std::cout << "================================================================================" << std::endl;
        //std::cout << "LocalTranslationMatrix " << Name() << " " << glm::to_string(LocalTranslationMatrix()) << std::endl;
        //std::cout << "LocalRotationMatrix    " << Name() << " " << glm::to_string(LocalRotationMatrix()) << std::endl;
        //std::cout << "LocalScaleMatrix       " << Name() << " " << glm::to_string(LocalScaleMatrix()) << std::endl;
        //std::cout << "LocalTransformMatrix   " << Name() << " " << glm::to_string(LocalTransformMatrix()) << std::endl;
        //std::cout << "worldTransformMatrix   " << Name() << " " << glm::to_string(worldTransformMatrix) << std::endl;
        //std::cout << "================================================================================" << std::endl;
        return worldTransformMatrix;
    };
    inline glm::mat4 WorldTranslationMatrix() const { return Parent() ? Parent()->WorldTransformMatrix() * LocalTranslationMatrix() : LocalTranslationMatrix(); };
    inline glm::mat4 WorldRotationMatrix() const { return Parent() ? Parent()->WorldTransformMatrix() * LocalRotationMatrix() : LocalRotationMatrix(); };
    inline glm::mat4 WorldScaleMatrix() const { return Parent() ? Parent()->WorldTransformMatrix() * LocalScaleMatrix() : LocalScaleMatrix(); };
    inline glm::mat4 LocalTransformMatrix() const { return LocalTranslationMatrix() * LocalRotationMatrix() * LocalScaleMatrix(); };
    inline glm::mat4 LocalTranslationMatrix() const { return glm::translate(Position()); };
    inline glm::mat4 LocalRotationMatrix() const { return glm::mat4_cast(Rotation()); };
    inline glm::mat4 LocalScaleMatrix() const { return glm::scale(Scale()); };
    inline void SetLocalTransform(glm::mat4 transform) {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transform, _scale, _rotation, _position, skew, perspective);
        //glm::vec3 scale;
        //glm::quat rotation;
        //glm::vec3 translation;
        //glm::vec3 skew;
        //glm::vec4 perspective;
        //glm::decompose(transform, scale, rotation, translation, skew, perspective);
        //SetPosition(translation);
        //SetRotation(glm::normalize(rotation));
        //SetScale(scale);
    };
    inline glm::vec3 WorldPosition() const { return Parent() ? Parent()->WorldTransformMatrix() * glm::vec4(Position(), 1.f) : Position(); }
    inline glm::quat WorldRotation() const { return Parent() ? Parent()->WorldTransformMatrix() * glm::mat4_cast(Rotation()) : Rotation(); }
    inline glm::vec3 WorldScale() const { return Parent() ? Parent()->WorldTransformMatrix() * glm::vec4(Scale(), 1.f) : Scale(); }
	/** @return the node local position */
    inline glm::vec3 Position() const { return _position; }
    /** @argument position : the node local position */
    inline void SetPosition(glm::vec3 position) { _position = position; };
    /** @return the node local rotation */
    inline glm::quat Rotation() const { return _rotation; };
    /** @argument rotation : the node local rotation */
    inline void SetRotation(glm::vec3 rotation) { _rotation = glm::quat(rotation); };
    /** @return the node local scale */
    inline void SetRotation(glm::quat rotation) { _rotation = rotation; };
    /** @return the node local scale */
    inline glm::vec3 Scale() const { return _scale; };
    /** @argument scale : the node local scale */
    inline void SetScale(glm::vec3 scale) { _scale = scale; };
    /**
     * @brief Common::Forward() * Rotation()
     * READONLY : Computed on demand
     */
    inline glm::vec3 Forward() const { return Rotation() * Common::Forward(); };
    /**
     * @brief READONLY : Computed on demand
     * @return Common::Up() * Rotation()
     */
    inline glm::vec3 Up() const { return Rotation() * Common::Up(); };
    /**
     * @brief READONLY : Computed on demand
     * Common::Right() * Rotation()
     */
    inline glm::vec3 Right() const { return Rotation() * Common::Right(); };
    inline void LookAt(const glm::vec3 &target, const glm::vec3 &up = Common::Up()) {
        auto direction(normalize(target - WorldPosition()));
        SetRotation(glm::quatLookAt(direction, up));
    };
    inline void LookAt(const std::shared_ptr<Transform> &target, const glm::vec3 &up = Common::Up()) {
        if (target == nullptr)
            return;
        LookAt(target->WorldPosition(), up);
    };
    inline std::shared_ptr<Transform> Parent() const { return _parent; }
    inline void SetParent(std::shared_ptr<Transform> parent) {
        if (parent == shared_from_this() || Parent() == parent)
            return;
        //if (Parent() != nullptr)
        //    Parent()->RemoveChild(shared_from_this());
        _parent = parent;
        //if (parent != nullptr)
        //    parent->AddChild(shared_from_this());
    }
    ~Transform() = default;

private:
    Transform(const std::string &name) : Object(name) {};
    /*
    void RemoveChild(std::shared_ptr<Transform> child) { _children.erase(std::remove(_children.begin(), _children.end(), child), _children.end()); };
    void AddChild(std::shared_ptr<Transform> child) {
        if (child == shared_from_this() || std::find(_children.begin(), _children.end(), child) != _children.end())
            return;
        _children.push_back(child);
        child->SetParent(shared_from_this());
    };
    */
    std::shared_ptr<Transform> _parent;
    //std::vector<std::shared_ptr<Transform>> _children;
	glm::vec3 _position { 0, 0, 0 };
    glm::quat _rotation /*{ 0, 0, 0, 1 }*/;
    glm::vec3 _scale { 1, 1, 1 };
};
