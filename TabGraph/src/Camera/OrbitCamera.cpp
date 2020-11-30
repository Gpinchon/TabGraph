/*
* @Author: gpi
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-12 15:49:14
*/

#include "Camera/OrbitCamera.hpp"
#include "Tools/Tools.hpp"
#include "Transform.hpp"

OrbitCamera::OrbitCamera(const std::string& iname, float ifov, float phi, float theta, float radius, Camera::Projection proj)
    : Camera(iname, proj)
{
    _phi = phi;
    _theta = theta;
    _radius = radius;
    SetFov(ifov);
}

std::shared_ptr<Transform> OrbitCamera::Target() const
{
    return _target.lock();
}

void OrbitCamera::_UpdateCPU(float)
{
    glm::vec3 targetPosition(0);
    if (Target() != nullptr)
        targetPosition = Target()->WorldPosition();
    SetPosition(targetPosition + Radius() * glm::vec3(sin(Phi()) * cos(Theta()), sin(Phi()) * sin(Theta()), cos(Phi())));
    LookAt(targetPosition);
}

float OrbitCamera::Phi() const
{
    return _phi;
}

void OrbitCamera::SetPhi(float phi)
{
    _phi = phi;
    SetNeedsUpdateCPU(true);
}

float OrbitCamera::Theta() const
{
    return _theta;
}

void OrbitCamera::SetTheta(float theta)
{
    _theta = theta;
    SetNeedsUpdateCPU(true);
}

float OrbitCamera::Radius() const
{
    return _radius;
}

void OrbitCamera::SetRadius(float radius)
{
    _radius = radius;
    SetNeedsUpdateCPU(true);
}

void OrbitCamera::SetTarget(const std::shared_ptr<Transform>& target)
{
    _target = target;
    SetNeedsUpdateCPU(true);
}
