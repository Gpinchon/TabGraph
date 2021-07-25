/*
* @Author: gpinchon
* @Date:   2019-07-16 08:53:02
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:46
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Cameras/Camera.hpp>
#include <Cameras/Projection.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Cameras {
class Orbit : public Core::Inherit<Camera, Orbit> {
public:
    READONLYPROPERTY(float, Phi, 0.f);
    READONLYPROPERTY(float, Theta, 0.f);
    READONLYPROPERTY(float, Radius, 0.f);

public:
    Orbit(const std::string&, float phi, float theta, float radius, Projection proj = Projection::PerspectiveInfinite());
    std::shared_ptr<Node> Target() const;
    void SetTarget(const std::shared_ptr<Node> target);
    void SetPhi(float);
    void SetTheta(float);
    void SetRadius(float);

private:
    virtual void _Update();
    std::weak_ptr<Node> _target;
};
}
