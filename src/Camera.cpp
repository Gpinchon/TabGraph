/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:25:02
*/

#include "Camera.hpp"
#include "Common.hpp"
#include "Window.hpp" // for Window
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h> // for sin, cos

Camera::Camera(const std::string& name, float ifov, CameraProjection proj)
    : Node(name)
{
    _fov = ifov;
    _projection_type = proj;
}

std::shared_ptr<Camera> Camera::Create(const std::string& name, float ifov, CameraProjection proj)
{
    return std::shared_ptr<Camera>(new Camera(name, ifov, proj));
}

std::shared_ptr<Camera> Camera::Create(std::shared_ptr<Camera> otherCamera)
{
    return std::shared_ptr<Camera>(new Camera(*otherCamera));
}

#include <glm/gtx/transform.hpp>

glm::vec3 Camera::Forward() const
{
    return Rotation() * Common::Forward();
}

glm::vec3 Camera::Right() const
{
    return Rotation() * Common::Right();
}

glm::vec3 Camera::Up() const
{
    return Rotation() * Common::Up();
}

void Camera::UpdateTransformMatrix()
{
    Node::UpdateTransformMatrix();
}

glm::mat4 Camera::ViewMatrix() const
{
    return glm::inverse(TransformMatrix());
}

void Camera::SetViewMatrix(glm::mat4 view)
{
    SetTransformMatrix(view);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    if (_projection_type == PerspectiveCamera)
        if (Zfar() > 0)
            return glm::perspective(glm::radians(Fov()), float(Window::size().x) / float(Window::size().y), Znear(), Zfar());
        else
            return glm::infinitePerspective(glm::radians(Fov()), float(Window::size().x) / float(Window::size().y), Znear());
    else
        return glm::ortho(_frustum.x, _frustum.y, _frustum.z, _frustum.w);
}

glm::ivec4 Camera::Frustum() const
{
    return _frustum;
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

void Camera::SetTransformMatrix(glm::mat4 transform)
{
    Node::SetTransformMatrix(transform);
}

glm::mat4 Camera::TransformMatrix() const
{
    return Node::TransformMatrix();
}