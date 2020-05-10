/*
* @Author: gpi
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 18:57:31
*/

#include "Camera/OrbitCamera.hpp"
#include "Tools.hpp"

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

void OrbitCamera::SetPosition(glm::vec3)
{

}

glm::vec3 OrbitCamera::Position() const
{
    glm::vec3 targetPosition(0);
    if (target() != nullptr)
        targetPosition = target()->Position();
    return targetPosition + Radius() * glm::vec3(sin(Phi()) * cos(Theta()), sin(Phi()) * sin(Theta()), cos(Phi()));
}

glm::quat OrbitCamera::Rotation() const
{
    glm::vec3 targetPosition(0);
    if (target() != nullptr)
        targetPosition = target()->Position();
    glm::vec3 forwardVector = normalize(targetPosition - Position());

    float dot = glm::dot(Common::Forward(), forwardVector);

    if (abs(dot - (-1.0f)) < 0.000001f)
    {
        return glm::quat(Common::Up().x, Common::Up().y, Common::Up().z, M_PI);
    }
    if (abs(dot - (1.0f)) < 0.000001f)
    {
        return glm::quat(0, 0, 0, 1);
    }

    float rotAngle = (float)acos(dot);
    glm::vec3 rotAxis = glm::cross(Common::Forward(), forwardVector);
    rotAxis = normalize(rotAxis);
    return glm::angleAxis(rotAngle, rotAxis);
}

float OrbitCamera::Phi() const
{
    return _phi;
}

void OrbitCamera::SetPhi(float phi)
{
    _phi = phi;
    SetNeedsTranformUpdate(true);
}

float OrbitCamera::Theta() const
{
    return _theta;
}

void OrbitCamera::SetTheta(float theta)
{
    _theta = theta;
    SetNeedsTranformUpdate(true);
}

float OrbitCamera::Radius() const
{
    return _radius;
}

void OrbitCamera::SetRadius(float radius)
{
    _radius = radius;
    SetNeedsTranformUpdate(true);
}
