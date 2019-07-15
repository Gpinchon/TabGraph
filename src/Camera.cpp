/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-15 11:34:47
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
    _cameras.push_back(camera);
    return (camera);
}

std::shared_ptr<Camera> Camera::Get(unsigned index)
{
    if (index >= _cameras.size())
        return (nullptr);
    return (_cameras.at(index));
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

glm::vec3 Camera::Forward()
{
    return Rotation();
}

void Camera::SetForward(glm::vec3 forward)
{
    SetRotation(forward);
}

glm::vec3 Camera::Right()
{
    return glm::cross(Common::Up(), Forward());
}

glm::vec3 Camera::Up()
{
    return glm::cross(Forward(), Right());
}

void Camera::set_current(std::shared_ptr<Camera> camera)
{
    _current = camera;
}

void Camera::transform_update()
{
    //Node::Update();
    if (target() != nullptr)
        SetTransformMatrix(glm::lookAt(Position(), target()->Position(), Up()));
    else
        SetTransformMatrix(glm::lookAt(Position(), Position() + Forward(), Up()));
    if (_projection_type == PerspectiveCamera) {
        auto size = Window::size();
        _projection = glm::perspective(_fov, float(size.x) / float(size.y), _znear, _zfar);
    } else
        _projection = glm::ortho(_frustum.x, _frustum.y, _frustum.z, _frustum.w);
}

glm::mat4 Camera::view()
{
    return TransformMatrix();
}

glm::mat4& Camera::projection()
{
    return _projection;
}

glm::ivec4& Camera::frustum()
{
    return _frustum;
}

float& Camera::fov()
{
    return _fov;
}

OrbitCamera::OrbitCamera(const std::string& iname, float ifov, float phi, float theta, float radius)
    : Camera(iname, ifov)
{
    _phi = phi;
    _theta = theta;
    _radius = radius;
}

std::shared_ptr<OrbitCamera> OrbitCamera::create(const std::string& iname, float ifov, float phi, float theta, float radius)
{
    std::shared_ptr<OrbitCamera> camera(new OrbitCamera(iname, ifov, phi, theta, radius));
    Node::add(camera);
    _cameras.push_back(std::static_pointer_cast<Camera>(camera));
    return (camera);
}

void OrbitCamera::orbite(float phi, float theta, float radius)
{
    glm::vec3 target_position(0, 0, 0);

    _phi = phi;
    _theta = theta;
    _radius = radius;
    if (target() != nullptr)
        target_position = target()->Position();
    SetPosition(target_position + _radius * glm::vec3(sin(_phi) * cos(_theta), sin(_phi) * sin(_theta), cos(_phi)));
}
