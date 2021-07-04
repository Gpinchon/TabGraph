/*
* @Author: gpinchon
* @Date:   2019-07-15 10:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:46
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Cameras/Camera.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
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
