/*
* @Author: gpinchon
* @Date:   2019-07-15 10:36:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:31
*/

#include "Camera/FPSCamera.hpp"
#include <memory>

FPSCamera::FPSCamera(const std::string& name, Camera::Projection proj)
    : Camera(name, proj)
{
}

static inline auto GetRotationFromYPR(const float Yaw, const float Pitch, const float Roll)
{
    auto radPitch(glm::radians(Pitch));
    auto radYaw(glm::radians(Yaw));
    auto radRoll(glm::radians(Roll));
    glm::quat quatRoll = glm::angleAxis(radRoll, Common::Forward());
    glm::quat quatPitch = glm::angleAxis(radPitch, Common::Right());
    glm::quat quatYaw = glm::angleAxis(radYaw, Common::Up());
    glm::quat rotation = quatYaw * quatPitch * quatRoll;

    return glm::normalize(rotation);
}

float FPSCamera::Yaw() const
{
    return _yaw;
}

void FPSCamera::SetYaw(float yaw)
{
    _yaw = yaw;
    SetRotation(GetRotationFromYPR(Yaw(), Pitch(), Roll()));
}

float FPSCamera::Pitch() const
{
    return _pitch;
}

void FPSCamera::SetPitch(float pitch)
{
    _pitch = pitch;
    SetRotation(GetRotationFromYPR(Yaw(), Pitch(), Roll()));
}

float FPSCamera::Roll() const
{
    return _roll;
}

void FPSCamera::SetRoll(float roll)
{
    _roll = roll;
    SetRotation(GetRotationFromYPR(Yaw(), Pitch(), Roll()));
}
