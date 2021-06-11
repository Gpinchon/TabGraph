/*
* @Author: gpinchon
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:30
*/

#include "Camera/OrbitCamera.hpp"
#include "Engine.hpp"
#include "Tools/Tools.hpp"
#include "Node.hpp"
#include "Renderer/Renderer.hpp"

OrbitCamera::OrbitCamera(const std::string& iname, float phi, float theta, float radius, Camera::Projection proj)
    : Camera(iname, proj)
{
    _phi = phi;
    _theta = theta;
    _radius = radius;
    _Update();
}

std::shared_ptr<Node> OrbitCamera::Target() const
{
    return _target.lock();
}

void OrbitCamera::_Update()
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
    if (phi == _phi)
        return;
    _phi = phi;
    _Update();
}

float OrbitCamera::Theta() const
{
    return _theta;
}

void OrbitCamera::SetTheta(float theta)
{
    if (theta == _theta)
        return;
    _theta = theta;
    _Update();
}

float OrbitCamera::Radius() const
{
    return _radius;
}

void OrbitCamera::SetRadius(float radius)
{
    if (radius == _radius)
        return;
    _radius = radius;
    _Update();
}

void OrbitCamera::SetTarget(const std::shared_ptr<Node> target)
{
    if (target == _target.lock())
        return;
    _target = target;
    _Update();
}
