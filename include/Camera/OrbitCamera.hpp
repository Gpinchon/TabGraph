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
    static std::shared_ptr<OrbitCamera> Create(const std::string&, float fov, float phi, float theta, float radius, Camera::Projection proj = Perspective);
    //virtual void UpdateViewMatrix() override;
    virtual glm::vec3 Position() const override;
    virtual glm::quat Rotation() const override;
    float Phi() const;
    void SetPhi(float);
    float Theta() const;
    void SetTheta(float);
    float Radius() const;
    void SetRadius(float);

private:
    OrbitCamera(const std::string&, float fov, float phi, float theta, float radius, Camera::Projection proj);
    virtual void SetPosition(glm::vec3) override;
    float _phi { 0 };
    float _theta { 0 };
    float _radius { 0 };
};
