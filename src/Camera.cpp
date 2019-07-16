/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-16 08:56:14
*/

#include "Camera.hpp"
#include "Common.hpp"
#include "Window.hpp" // for Window
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h> // for sin, cos

std::vector<std::shared_ptr<Camera>> Camera::_cameras;
std::weak_ptr<Camera> Camera::_current;

Camera::Camera(const std::string& name, float ifov, CameraProjection proj)
    : Node(name)
{
    _fov = ifov;
    _projection_type = proj;
}

std::shared_ptr<Camera> Camera::create(const std::string& name, float ifov, CameraProjection proj)
{
    std::shared_ptr<Camera> camera(new Camera(name, ifov, proj));
    Node::add(camera);
    Camera::add(camera);
    //_cameras.push_back(camera);
    return (camera);
}

std::shared_ptr<Camera> Camera::Get(unsigned index)
{
    if (index >= _cameras.size())
        return (nullptr);
    return (_cameras.at(index));
}

void Camera::add(std::shared_ptr<Camera> camera)
{
    _cameras.push_back(camera);
}

std::shared_ptr<Camera> Camera::get_by_name(const std::string& name)
{
    for (auto n : _cameras) {
        if (name == n->name())
            return (n);
    }
    return (nullptr);
}

std::shared_ptr<Camera> Camera::current()
{
    return (_current.lock());
}

glm::vec3 Camera::Forward() const
{
    return Rotation();
}

void Camera::SetForward(glm::vec3 forward)
{
    SetRotation(forward);
}

glm::vec3 Camera::Right() const
{
    return glm::cross(Common::Up(), Forward());
}

glm::vec3 Camera::Up() const
{
    return glm::cross(Forward(), Right());
}

void Camera::set_current(std::shared_ptr<Camera> camera)
{
    _current = camera;
}

void Camera::UpdateViewMatrix()
{
    UpdateTranslationMatrix();
    UpdateRotationMatrix();
    SetViewMatrix(TranslationMatrix() * RotationMatrix());
}

void Camera::UpdateProjectionMatrix()
{
    if (_projection_type == PerspectiveCamera)
        SetProjectionMatrix(glm::perspective(_fov, float(Window::size().x) / float(Window::size().y), _znear, _zfar));
    else
        SetProjectionMatrix(glm::ortho(_frustum.x, _frustum.y, _frustum.z, _frustum.w));
}

void Camera::UpdateTransformMatrix()
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

glm::mat4 Camera::ViewMatrix() const
{
    return TransformMatrix();
}

void Camera::SetViewMatrix(glm::mat4 view)
{
    SetTransformMatrix(view);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    return _projection;
}

void Camera::SetProjectionMatrix(glm::mat4 projection)
{
    _projection = projection;
}

glm::ivec4& Camera::frustum()
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
