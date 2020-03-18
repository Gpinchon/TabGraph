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
    return std::shared_ptr<FPSCamera>(new FPSCamera(name, ifov, proj));
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

glm::quat FPSCamera::Rotation() const
{
    auto radPitch(glm::radians(Pitch()));
    auto radYaw(glm::radians(Yaw()));
    auto radRoll(glm::radians(Roll()));

    glm::quat qPitch = glm::angleAxis(radPitch, Common::Right());
    glm::quat qYaw =  glm::angleAxis(radYaw, Common::Up());
    glm::quat qRoll = glm::angleAxis(radRoll, Common::Forward());
    glm::quat rotate = qYaw * qPitch * qRoll;

    return glm::normalize(rotate);
}
