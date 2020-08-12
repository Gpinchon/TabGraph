/*
* @Author: gpi
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-11 22:48:39
*/

#include "Camera/OrbitCamera.hpp"
#include "Tools.hpp"
#include "Transform.hpp"

OrbitCamera::OrbitCamera(const std::string& iname, float ifov, float phi, float theta, float radius, Camera::Projection proj)
    : Camera(iname, ifov, proj)
{
    _phi = phi;
    _theta = theta;
    _radius = radius;
}

std::shared_ptr<OrbitCamera> OrbitCamera::Create(const std::string& iname, float ifov, float phi, float theta, float radius, Camera::Projection proj)
{
    std::shared_ptr<OrbitCamera> camera(new OrbitCamera(iname, ifov, phi, theta, radius, proj));
    return (camera);
}

std::shared_ptr<Node> OrbitCamera::Target() const
{
    return _target.lock();
}

void OrbitCamera::_UpdateCPU(float)
{
    glm::vec3 targetPosition(0);
    if (Target() != nullptr && Target()->GetComponent<Transform>() != nullptr)
        targetPosition = Target()->GetComponent<Transform>()->WorldPosition();
    GetComponent<Transform>()->SetPosition(targetPosition + Radius() * glm::vec3(sin(Phi()) * cos(Theta()), sin(Phi()) * sin(Theta()), cos(Phi())));
    GetComponent<Transform>()->LookAt(targetPosition);
}

float OrbitCamera::Phi() const
{
    return _phi;
}

void OrbitCamera::SetPhi(float phi)
{
    _phi = phi;
}

float OrbitCamera::Theta() const
{
    return _theta;
}

void OrbitCamera::SetTheta(float theta)
{
    _theta = theta;
}

float OrbitCamera::Radius() const
{
    return _radius;
}

void OrbitCamera::SetRadius(float radius)
{
    _radius = radius;
}

void OrbitCamera::SetTarget(const std::shared_ptr<Node>& target)
{
    _target = target;
}
