/*
* @Author: gpinchon
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 10:01:35
*/

#include <Camera/OrbitCamera.hpp>
#include <Engine.hpp>
#include <Nodes/Node.hpp>
#include <Tools/Tools.hpp>

namespace TabGraph::Cameras {
Orbit::Orbit(const std::string& iname, float phi, float theta, float radius, Camera::Projection proj)
    : Inherit(iname, proj)
{
    _phi = phi;
    _theta = theta;
    _radius = radius;
    _Update();
}

std::shared_ptr<Nodes::Node> Orbit::Target() const
{
    return _target.lock();
}

void Orbit::_Update()
{
    glm::vec3 targetPosition(0);
    if (Target() != nullptr)
        targetPosition = Target()->GetWorldPosition();
    //TODO Add SetWorldPosition to Node
    SetLocalPosition(targetPosition + Radius() * glm::vec3(sin(Phi()) * cos(Theta()), sin(Phi()) * sin(Theta()), cos(Phi())));
    LookAt(targetPosition);
}

float Orbit::Phi() const
{
    return _phi;
}

void Orbit::SetPhi(float phi)
{
    if (phi == _phi)
        return;
    _phi = phi;
    _Update();
}

float Orbit::Theta() const
{
    return _theta;
}

void Orbit::SetTheta(float theta)
{
    if (theta == _theta)
        return;
    _theta = theta;
    _Update();
}

float Orbit::Radius() const
{
    return _radius;
}

void Orbit::SetRadius(float radius)
{
    if (radius == _radius)
        return;
    _radius = radius;
    _Update();
}

void Orbit::SetTarget(const std::shared_ptr<Node> target)
{
    if (target == _target.lock())
        return;
    _target = target;
    _Update();
}
}