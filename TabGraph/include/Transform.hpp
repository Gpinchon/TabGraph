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
public:
    Transform() : Component() {};
    Transform(const std::string& name) : Component(name) {};
    static std::shared_ptr<Transform> Create();
    glm::vec3 TransformPoint(glm::vec3 position) { return glm::vec4(position, 1.f) * WorldTransformMatrix(); };
    glm::vec3 operator()(glm::vec3 position) { return TransformPoint(position); };
    glm::mat4 WorldTransformMatrix();
    glm::mat4 WorldTranslationMatrix() const;
    glm::mat4 WorldRotationMatrix() const;
    glm::mat4 WorldScaleMatrix() const;
    glm::mat4 LocalTransformMatrix();
    glm::mat4 LocalTranslationMatrix() const;
    glm::mat4 LocalRotationMatrix() const;
    glm::mat4 LocalScaleMatrix() const;
    glm::vec3 WorldPosition() const;
    glm::quat WorldRotation() const;
    glm::vec3 WorldScale() const;
    /** @return the node local position */
    glm::vec3 Position() const;
    /** @argument position : the node local position */
    void SetPosition(glm::vec3 position);
    /** @return the node local rotation */
    glm::quat Rotation() const;
    /** @argument rotation : the node local rotation */
    void SetRotation(glm::vec3 rotation);
    /** @return the node local scale */
    void SetRotation(glm::quat rotation);
    /** @return the node local scale */
    glm::vec3 Scale() const;
    /** @argument scale : the node local scale */
    void SetScale(glm::vec3 scale);
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
    std::shared_ptr<Transform> Parent() const;
    void SetParent(std::shared_ptr<Transform> parent);
    ~Transform() = default;

private:
    virtual std::shared_ptr<Component> _Clone() const override {
        return tools::make_shared<Transform>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float) override {};
    virtual void _UpdateGPU(float) override {};
    virtual void _FixedUpdateCPU(float) override {};
    virtual void _FixedUpdateGPU(float) override {};
    std::shared_ptr<Transform> _transformParent;
    glm::vec3 _position { 0, 0, 0 };
    glm::quat _rotation { glm::vec3(0.0, 0.0, 0.0) };
    glm::vec3 _scale { 1, 1, 1 };
    bool _needsUpdate { true };
    glm::mat4 _localTransformMatrix { glm::mat4(1) };
};
