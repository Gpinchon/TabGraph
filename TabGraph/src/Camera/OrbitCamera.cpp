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
#include "Render.hpp"

OrbitCamera::OrbitCamera(const std::string& iname, float ifov, float phi, float theta, float radius, Camera::Projection proj)
    : Camera(iname, proj)
{
    _phi = phi;
    _theta = theta;
    _radius = radius;
    SetFov(ifov);
    _Update(0);
}

std::shared_ptr<Node> OrbitCamera::Target() const
{
    return _target.lock();
}

void OrbitCamera::_Update(float)
{
    glm::vec3 targetPosition(0);
    if (Target() != nullptr)
        targetPosition = Target()->WorldPosition();
    SetPosition(targetPosition + Radius() * glm::vec3(sin(Phi()) * cos(Theta()), sin(Phi()) * sin(Theta()), cos(Phi())));
    LookAt(targetPosition);
    _updateSlot.Disconnect();
}

float OrbitCamera::Phi() const
{
    return _phi;
}

void OrbitCamera::SetPhi(float phi)
{
    _phi = phi;
    if (!_updateSlot.Connected())
        _updateSlot = Render::OnBeforeRender().ConnectMember(this, &OrbitCamera::_Update);
}

float OrbitCamera::Theta() const
{
    return _theta;
}

void OrbitCamera::SetTheta(float theta)
{
    _theta = theta;
    if (!_updateSlot.Connected())
        _updateSlot = Render::OnBeforeRender().ConnectMember(this, &OrbitCamera::_Update);
}

float OrbitCamera::Radius() const
{
    return _radius;
}

void OrbitCamera::SetRadius(float radius)
{
    _radius = radius;
    if (!_updateSlot.Connected())
        _updateSlot = Render::OnBeforeRender().ConnectMember(this, &OrbitCamera::_Update);
}

void OrbitCamera::SetTarget(const std::shared_ptr<Node>& target)
{
    _target = target;
    if (!_updateSlot.Connected())
        _updateSlot = Render::OnBeforeRender().ConnectMember(this, &OrbitCamera::_Update);
}
