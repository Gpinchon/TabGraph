#pragma once
#include "Common.hpp"
#include "Component.hpp"
#include <algorithm>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <vector>

/** Header-only class for performance */
class Transform : public Component {
    /** The local position */
    PROPERTY(glm::vec3, Position, 0);
    /** The local rotation */
    PROPERTY(glm::quat, Rotation, glm::vec3(0.0, 0.0, 0.0));
    /** The local scale */
    PROPERTY(glm::vec3, Scale, 1);
public:
    Transform();
    Transform(const Transform& transform);
    Transform(const std::string& name);
    glm::vec3 TransformPoint(glm::vec3 position) { return glm::vec4(position, 1.f) * WorldTransformMatrix(); };
    glm::vec3 operator()(glm::vec3 position) { return TransformPoint(position); };

    glm::mat4 WorldTransformMatrix();
    glm::mat4 WorldTranslationMatrix();
    glm::mat4 WorldRotationMatrix();
    glm::mat4 WorldScaleMatrix();

    Signal<glm::mat4> LocalTransformMatrixChanged;
    Signal<glm::mat4> LocalTranslationMatrixChanged;
    Signal<glm::mat4> LocalRotationMatrixChanged;
    Signal<glm::mat4> LocalScaleMatrixChanged;
    glm::mat4 GetLocalTransformMatrix();
    glm::mat4 GetLocalTranslationMatrix();
    glm::mat4 GetLocalRotationMatrix();
    glm::mat4 GetLocalScaleMatrix();

    //Signal<> WorldTransformChanged;
    glm::vec3 WorldPosition() const;
    glm::quat WorldRotation() const;
    glm::vec3 WorldScale() const;
    /** @argument rotation : the node local rotation */
    void SetRotation(glm::vec3 rotation);
    /**
     * @brief Common::Forward() * Rotation()
     * READONLY : Computed on demand
     */
    glm::vec3 Forward() const;
    /**
     * @brief READONLY : Computed on demand
     * @return Common::Up() * Rotation()
     */
    glm::vec3 Up() const;
    /**
     * @brief READONLY : Computed on demand
     * Common::Right() * Rotation()
     */
    glm::vec3 Right() const;
    void LookAt(const glm::vec3& target, const glm::vec3& up = Common::Up());
    void LookAt(const std::shared_ptr<Transform>& target, const glm::vec3& up = Common::Up());
    Signal<std::shared_ptr<Transform>> ParentChanged;
    std::shared_ptr<Transform> GetParent() const;
    void SetParent(std::shared_ptr<Transform> parent);
    ~Transform() = default;

private:
    virtual std::shared_ptr<Component> _Clone() override {
        auto clone = Component::Create<Transform>(*this);
        return clone;
    }
    virtual void _Replace(const std::shared_ptr<Component> &oldComponent, const std::shared_ptr<Component> &newComponent) override {
        if (GetParent() == oldComponent)
            SetParent(std::static_pointer_cast<Transform>(newComponent));
    };
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float) override {};
    virtual void _FixedUpdateCPU(float) override;;
    void _SetLocalTransformMatrix(const glm::mat4& matrix);
    void _SetLocalTranslationMatrix(const glm::mat4& matrix);
    void _SetLocalRotationMatrix(const glm::mat4& matrix);
    void _SetLocalScaleMatrix(const glm::mat4& matrix);
    void _OnPositionChanged(glm::vec3 position) {
        _positionChanged = true;
    }
    void _OnRotationChanged(glm::quat rotation) {
        _rotationChanged = true;
    }
    void _OnScaleChanged(glm::vec3 scale) {
        _scaleChanged = true;
    }
    bool _positionChanged{ true };
    bool _rotationChanged{ true };
    bool _scaleChanged{ true };

    glm::mat4 _localTransformMatrix{ glm::mat4(1) };
    glm::mat4 _localTranslationMatrix{ glm::translate(glm::vec3(0)) };
    glm::mat4 _localRotationMatrix{ glm::rotate(0.f, glm::vec3(0)) };
    glm::mat4 _localScaleMatrix{ glm::scale(glm::vec3(1)) };
    std::weak_ptr<Transform> _parent;
};
