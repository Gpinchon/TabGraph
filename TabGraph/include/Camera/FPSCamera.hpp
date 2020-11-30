/*
* @Author: gpi
* @Date:   2019-07-15 10:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:08
*/

#pragma once

#include "Camera/Camera.hpp"
#include <glm/vec3.hpp>

class FPSCamera : public Camera {
public:
    FPSCamera(const std::string& name, float fov, Camera::Projection proj = Projection::Perspective);
    virtual float Yaw() const;
    virtual void SetYaw(float);
    virtual float Pitch() const;
    virtual void SetPitch(float);
    virtual float Roll() const;
    virtual void SetRoll(float);
    //virtual glm::quat Rotation() const override;

protected:
    float _yaw { 0 };
    float _pitch { 0 };
    float _roll { 0 };
private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<FPSCamera>(*this);
    }
};