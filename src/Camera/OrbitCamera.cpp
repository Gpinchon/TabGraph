/*
* @Author: gpi
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-08 14:08:41
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
    return nullptr;
}

void OrbitCamera::UpdateTransform()
{
    glm::vec3 targetPosition(0);
    if (Target() != nullptr && Target()->GetComponent<Transform>() != nullptr)
        targetPosition = Target()->GetComponent<Transform>()->WorldPosition();
    GetComponent<Transform>()->SetPosition(targetPosition + Radius() * glm::vec3(sin(Phi()) * cos(Theta()), sin(Phi()) * sin(Theta()), cos(Phi())));
    glm::vec3 forwardVector = normalize(targetPosition - GetComponent<Transform>()->WorldPosition());

    float dot = glm::dot(Common::Forward(), forwardVector);

    if (fabs(dot - (-1.0f)) < 0.000001f) {
        GetComponent<Transform>()->SetRotation(glm::quat(Common::Up().x, Common::Up().y, Common::Up().z, M_PI));
        return;
    }
    if (fabs(dot - (1.0f)) < 0.000001f) {
        GetComponent<Transform>()->SetRotation(glm::quat(0, 0, 0, 1));
        return;
    }

    float rotAngle = (float)acos(dot);
    glm::vec3 rotAxis = glm::cross(Common::Forward(), forwardVector);
    rotAxis = normalize(rotAxis);
    GetComponent<Transform>()->SetRotation(glm::angleAxis(rotAngle, rotAxis));
}

float OrbitCamera::Phi() const
{
    return _phi;
}

void OrbitCamera::SetPhi(float phi)
{
    _phi = phi;
    UpdateTransform();
    //SetNeedsTranformUpdate(true);
}

float OrbitCamera::Theta() const
{
    return _theta;
}

void OrbitCamera::SetTheta(float theta)
{
    _theta = theta;
    UpdateTransform();
    //SetNeedsTranformUpdate(true);
}

float OrbitCamera::Radius() const
{
    return _radius;
}

void OrbitCamera::SetRadius(float radius)
{
    _radius = radius;
    UpdateTransform();
    //SetNeedsTranformUpdate(true);
}
