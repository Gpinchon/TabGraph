/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-21 21:54:14
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Cameras/Projection.hpp>
#include <Common.hpp>
#include <Core/Inherit.hpp>
#include <Nodes/Node.hpp>

#include <array>
#include <functional>
#include <glm/vec3.hpp>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Cameras {
/**
* @brief The default "general purpose" camera
*/
class Camera : public Core::Inherit<Nodes::Node, Camera> {
public:
    Camera(Projection = Projection::PerspectiveInfinite());
    Camera(const std::string& name, Projection = Projection::PerspectiveInfinite());
    ~Camera() = default;

    /**
        * @brief alias for inverse TransformMatrix
        * @return the camera's view matrix
        */
    glm::mat4 GetViewMatrix();
    /**
        * @brief Computes the camera frustum's 8 corners
        * @return the camera frustum's 8 corners in world space
    */
    std::array<glm::vec3, 8> ExtractFrustum();

    inline const Projection& GetProjection() const
    {
        return _projection;
    }
    inline void SetProjection(const Projection& projectionType)
    {
        _projection = projectionType;
    }

private:
    Projection _projection;
};
}
