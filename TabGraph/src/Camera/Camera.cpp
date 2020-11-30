/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-08 14:05:26
*/

#include "Camera/Camera.hpp"
#include "Common.hpp"
#include "Transform.hpp"
#include "Window.hpp" // for Window
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h> // for sin, cos

Camera::Camera(const std::string& name, Camera::Projection proj)
    : Node(name)
{
    _projection_type = proj;
}

glm::mat4 Camera::ViewMatrix()
{
    return glm::inverse(WorldTransformMatrix());
}

glm::mat4 Camera::ProjectionMatrix() const
{
    if (ProjectionType() == Camera::Projection::Perspective) {
        if (Zfar() > 0)
            return glm::perspective(glm::radians(Fov()), float(Window::size().x) / float(Window::size().y), Znear(), Zfar());
        else
            return glm::infinitePerspective(glm::radians(Fov()), float(Window::size().x) / float(Window::size().y), Znear());
    } else
        return glm::ortho(_frustum.x, _frustum.y, _frustum.z, _frustum.w, _znear, _zfar);
}

glm::vec4 Camera::Frustum() const
{
    return _frustum;
}

void Camera::SetFrustum(glm::vec4 frustum)
{
    _frustum = frustum;
}

float Camera::Fov() const
{
    return _fov;
}

void Camera::SetFov(float fov)
{
    _fov = fov;
}

float Camera::Znear() const
{
    return _znear;
}

void Camera::SetZnear(float znear)
{
    _znear = znear;
}

float Camera::Zfar() const
{
    return _zfar;
}

void Camera::SetZfar(float zfar)
{
    _zfar = zfar;
}

Camera::Projection Camera::ProjectionType() const
{
    return _projection_type;
}

void Camera::SetProjectionType(Camera::Projection projectionType)
{
    _projection_type = projectionType;
}
