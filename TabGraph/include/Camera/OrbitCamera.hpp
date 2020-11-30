/*
* @Author: gpi
* @Date:   2019-07-16 08:53:02
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:09
*/

#pragma once

#include "Camera/Camera.hpp"

class OrbitCamera : public Camera {
public:
    OrbitCamera(const std::string&, float fov, float phi, float theta, float radius, Camera::Projection proj = Projection::Perspective);
    std::shared_ptr<Transform> Target() const;
    void SetTarget(const std::shared_ptr<Transform>& target);
    float Phi() const;
    void SetPhi(float);
    float Theta() const;
    void SetTheta(float);
    float Radius() const;
    void SetRadius(float);

private:
    virtual void _Replace(const std::shared_ptr<Component>& oldComponent, const std::shared_ptr<Component>& newComponent) override {
        if (oldComponent == Target())
            SetTarget(std::static_pointer_cast<Node>(newComponent));
    }
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<OrbitCamera>(*this);
    }
    virtual void _UpdateCPU(float delta) override;
    float _phi { 0 };
    float _theta { 0 };
    float _radius { 0 };
    std::weak_ptr<Transform> _target;
};
