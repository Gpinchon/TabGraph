/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:31
*/

#include "Camera/Camera.hpp"
#include "Common.hpp"
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

#include "Render.hpp"

glm::mat4 Camera::ProjectionMatrix() const
{
    glm::mat4 proj;
    if (ProjectionType() == Camera::Projection::Perspective) {
        if (Zfar() > 0)
            proj = glm::perspective(glm::radians(Fov()), float(Window::size().x) / float(Window::size().y), Znear(), Zfar());
        else
            proj = glm::infinitePerspective(glm::radians(Fov()), float(Window::size().x) / float(Window::size().y), Znear());
    } else
        proj = glm::ortho(_frustum.x, _frustum.y, _frustum.z, _frustum.w, _znear, _zfar);
    switch (Render::FrameNumber() % 4) {
    case 0:
        proj[2][0] += 0.25 / float(Window::size().x);
        proj[2][1] += 0.25 / float(Window::size().y);
        break;
    case 2:
        proj[2][0] -= 0.25 / float(Window::size().x);
        proj[2][1] -= 0.25 / float(Window::size().y);
        break;
        /*
    case 0:
        proj[2][0] += 0.5 / float(Window::size().x);
        proj[2][1] += 0.5 / float(Window::size().y);
        break;
    case 1:
        //proj[2][0] -= 0.5 / float(Window::size().x);
        proj[2][1] += 0.5 / float(Window::size().y);
        break;
    case 2:
        proj[2][0] -= 0.5 / float(Window::size().x);
        proj[2][1] += 0.5 / float(Window::size().y);
    case 3:
        proj[2][0] += 0.5 / float(Window::size().x);
        //proj[2][1] += 0.5 / float(Window::size().y);
        break;
    case 4:
        break;
    case 5:
        proj[2][0] -= 0.5 / float(Window::size().x);
        //proj[2][1] += 0.5 / float(Window::size().y);
        break;
    case 6:
        //proj[2][0] -= 0.5 / float(Window::size().x);
        proj[2][1] -= 0.5 / float(Window::size().y);
        break;
    case 7:
        proj[2][0] += 0.5 / float(Window::size().x);
        proj[2][1] -= 0.5 / float(Window::size().y);
    case 8:
        proj[2][0] -= 0.5 / float(Window::size().x);
        proj[2][1] -= 0.5 / float(Window::size().y);
        break;
    */
    }
    return proj;
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
