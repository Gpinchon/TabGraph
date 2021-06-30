/*
* @Author: gpinchon
* @Date:   2019-07-15 10:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:32
*/

#pragma once

#include <Core/Inherit.hpp>
#include <Camera/Camera.hpp>
#include <Property.hpp>

namespace TabGraph::Cameras {
class FPS : public Core::Inherit<Camera, FPS> {
    READONLYPROPERTY(float, Yaw, 0);
    READONLYPROPERTY(float, Pitch, 0);
    READONLYPROPERTY(float, Roll, 0);

public:
    FPS(const std::string& name, Camera::Projection proj = Projection::PerspectiveInfinite());
    void SetYaw(float);
    void SetPitch(float);
    void SetRoll(float);
};
}
