/*
* @Author: gpi
* @Date:   2019-07-15 10:36:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-08 14:08:13
*/

#include "Camera/FPSCamera.hpp"
#include "Transform.hpp"
#include <memory>

FPSCamera::FPSCamera(const std::string& name, float fov, Camera::Projection proj)
    : Camera(name, fov, proj)
{
}

std::shared_ptr<FPSCamera> FPSCamera::Create(const std::string& name, float ifov, Camera::Projection proj)
{
    return tools::make_shared<FPSCamera>(name, ifov, proj);
}

static inline auto GetRotation(const float Yaw, const float Pitch, const float Roll)
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
    SetRotation(GetRotation(Yaw(), Pitch(), Roll()));
    //SetNeedsTranformUpdate(true);
}

float FPSCamera::Pitch() const
{
    return _pitch;
}

void FPSCamera::SetPitch(float pitch)
{
    _pitch = pitch;
    SetRotation(GetRotation(Yaw(), Pitch(), Roll()));
    //SetNeedsTranformUpdate(true);
}

float FPSCamera::Roll() const
{
    return _roll;
}

void FPSCamera::SetRoll(float roll)
{
    _roll = roll;
    SetRotation(GetRotation(Yaw(), Pitch(), Roll()));
    //SetNeedsTranformUpdate(true);
}

/*glm::quat FPSCamera::Rotation() const
{
    auto radPitch(glm::radians(Pitch()));
    auto radYaw(glm::radians(Yaw()));
    auto radRoll(glm::radians(Roll()));
    glm::quat quatRoll = glm::angleAxis(radRoll, Common::Forward());
    glm::quat quatPitch = glm::angleAxis(radPitch, Common::Right());
    glm::quat quatYaw =  glm::angleAxis(radYaw, Common::Up());
    glm::quat rotation = quatYaw * quatPitch * quatRoll;

    return glm::normalize(rotation);
}*/
