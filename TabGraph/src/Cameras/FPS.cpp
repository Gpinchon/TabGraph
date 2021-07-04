/*
* @Author: gpinchon
* @Date:   2019-07-15 10:36:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:31
*/

#include <Cameras/FPS.hpp>

namespace TabGraph::Cameras {
FPS::FPS(const std::string& name, Camera::Projection proj)
    : Inherit(name, proj)
{
}

static inline glm::quat GetRotationFromYPR(const float Yaw, const float Pitch, const float Roll)
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

void FPS::SetYaw(float yaw)
{
    _SetYaw(yaw);
    SetLocalRotation(GetRotationFromYPR(GetYaw(), GetPitch(), GetRoll()));
}

void FPS::SetPitch(float pitch)
{
    _SetPitch(pitch);
    SetLocalRotation(GetRotationFromYPR(GetYaw(), GetPitch(), GetRoll()));
}

void FPS::SetRoll(float roll)
{
    _SetRoll(roll);
    SetLocalRotation(GetRotationFromYPR(GetYaw(), GetPitch(), GetRoll()));
}
}