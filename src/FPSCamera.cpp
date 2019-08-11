/*
* @Author: gpi
* @Date:   2019-07-15 10:36:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:24:43
*/

#include "FPSCamera.hpp"
#include <memory>

FPSCamera::FPSCamera(const std::string& name, float fov, CameraProjection proj)
    : Camera(name, fov, proj)
{
}

std::shared_ptr<FPSCamera> FPSCamera::Create(const std::string& name, float ifov, CameraProjection proj)
{
    std::shared_ptr<FPSCamera> camera(new FPSCamera(name, ifov, proj));
    Camera::Add(camera);
    return (camera);
}

float FPSCamera::Yaw() const
{
    return _yaw;
}

void FPSCamera::SetYaw(float yaw)
{
    _yaw = yaw;
}

float FPSCamera::Pitch() const
{
    return _pitch;
}

void FPSCamera::SetPitch(float pitch)
{
    _pitch = pitch;
}

float FPSCamera::Roll() const
{
    return _roll;
}

void FPSCamera::SetRoll(float roll)
{
    _roll = roll;
}

glm::vec3 FPSCamera::Forward() const
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(Pitch())) * cos(glm::radians(Yaw()));
    direction.y = sin(glm::radians(Pitch()));
    direction.z = cos(glm::radians(Pitch())) * sin(glm::radians(Yaw()));
    return direction;
}

void FPSCamera::UpdateViewMatrix()
{
    if (target() != nullptr)
        SetViewMatrix(glm::lookAt(Position(), target()->Position(), Up()));
    else
        SetViewMatrix(glm::lookAt(Position(), Position() + Forward(), Up()));
}
