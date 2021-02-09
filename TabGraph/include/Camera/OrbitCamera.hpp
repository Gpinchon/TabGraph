/*
* @Author: gpinchon
* @Date:   2019-07-16 08:53:02
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:31
*/

#pragma once

#include "Camera/Camera.hpp"
#include "Event/Signal.hpp"

class OrbitCamera : public Camera {
public:
    OrbitCamera(const std::string&, float fov, float phi, float theta, float radius, Camera::Projection proj = Projection::Perspective);
    std::shared_ptr<Node> Target() const;
    void SetTarget(const std::shared_ptr<Node>& target);
    float Phi() const;
    void SetPhi(float);
    float Theta() const;
    void SetTheta(float);
    float Radius() const;
    void SetRadius(float);

private:
    virtual void _Replace(const std::shared_ptr<Component>& oldComponent, const std::shared_ptr<Component>& newComponent) override
    {
        if (oldComponent == Target())
            SetTarget(std::static_pointer_cast<Node>(newComponent));
    }
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<OrbitCamera>(*this);
    }
    virtual void _Update(float delta);
    Signal<float>::ScoppedSlot _updateSlot;
    float _phi { 0 };
    float _theta { 0 };
    float _radius { 0 };
    std::weak_ptr<Node> _target;
};
