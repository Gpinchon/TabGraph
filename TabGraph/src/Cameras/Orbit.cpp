/*
* @Author: gpinchon
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 10:01:35
*/

#include <Cameras/Orbit.hpp>
#include <Nodes/Node.hpp>

namespace TabGraph::Cameras {
Orbit::Orbit(const std::string& iname, float phi, float theta, float radius, Camera::Projection proj)
    : Inherit(iname, proj)
{
    _Phi = phi;
    _Theta = theta;
    _Radius = radius;
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
    SetLocalPosition(targetPosition + GetRadius() * glm::vec3(sin(GetPhi()) * cos(GetTheta()), sin(GetPhi()) * sin(GetTheta()), cos(GetPhi())));
    LookAt(targetPosition);
}

void Orbit::SetPhi(float phi)
{
    if (phi == GetPhi())
        return;
    _SetPhi(phi);
    _Update();
}

void Orbit::SetTheta(float theta)
{
    if (theta == GetTheta())
        return;
    _SetTheta(theta);
    _Update();
}

void Orbit::SetRadius(float radius)
{
    if (radius == GetRadius())
        return;
    _SetRadius(radius);
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