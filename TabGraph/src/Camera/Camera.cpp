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

//Halton sequence scaled to [-1:1]
static const glm::vec2 haltonSequence[64] = {
    glm::vec2(0, -0.333333),
    glm::vec2(-0.5, 0.333333),
    glm::vec2(0.5, -0.777778),
    glm::vec2(-0.75, -0.111111),
    glm::vec2(0.25, 0.555556),
    glm::vec2(-0.25, -0.555556),
    glm::vec2(0.75, 0.111111),
    glm::vec2(-0.875, 0.777778),
    glm::vec2(0.125, -0.925926),
    glm::vec2(-0.375, -0.259259),
    glm::vec2(0.625, 0.407407),
    glm::vec2(-0.625, -0.703704),
    glm::vec2(0.375, -0.037037),
    glm::vec2(-0.125, 0.62963),
    glm::vec2(0.875, -0.481481),
    glm::vec2(-0.9375, 0.185185),
    glm::vec2(0.0625, 0.851852),
    glm::vec2(-0.4375, -0.851852),
    glm::vec2(0.5625, -0.185185),
    glm::vec2(-0.6875, 0.481482),
    glm::vec2(0.3125, -0.62963),
    glm::vec2(-0.1875, 0.037037),
    glm::vec2(0.8125, 0.703704),
    glm::vec2(-0.8125, -0.407407),
    glm::vec2(0.1875, 0.259259),
    glm::vec2(-0.3125, 0.925926),
    glm::vec2(0.6875, -0.975309),
    glm::vec2(-0.5625, -0.308642),
    glm::vec2(0.4375, 0.358025),
    glm::vec2(-0.0625, -0.753086),
    glm::vec2(0.9375, -0.0864198),
    glm::vec2(-0.96875, 0.580247),
    glm::vec2(0.03125, -0.530864),
    glm::vec2(-0.46875, 0.135803),
    glm::vec2(0.53125, 0.802469),
    glm::vec2(-0.71875, -0.901235),
    glm::vec2(0.28125, -0.234568),
    glm::vec2(-0.21875, 0.432099),
    glm::vec2(0.78125, -0.679012),
    glm::vec2(-0.84375, -0.0123457),
    glm::vec2(0.15625, 0.654321),
    glm::vec2(-0.34375, -0.45679),
    glm::vec2(0.65625, 0.209877),
    glm::vec2(-0.59375, 0.876543),
    glm::vec2(0.40625, -0.82716),
    glm::vec2(-0.09375, -0.160494),
    glm::vec2(0.90625, 0.506173),
    glm::vec2(-0.90625, -0.604938),
    glm::vec2(0.09375, 0.0617284),
    glm::vec2(-0.40625, 0.728395),
    glm::vec2(0.59375, -0.382716),
    glm::vec2(-0.65625, 0.283951),
    glm::vec2(0.34375, 0.950617),
    glm::vec2(-0.15625, -0.950617),
    glm::vec2(0.84375, -0.283951),
    glm::vec2(-0.78125, 0.382716),
    glm::vec2(0.21875, -0.728395),
    glm::vec2(-0.28125, -0.0617284),
    glm::vec2(0.71875, 0.604938),
    glm::vec2(-0.53125, -0.506173),
    glm::vec2(0.46875, 0.160494),
    glm::vec2(-0.03125, 0.82716),
    glm::vec2(0.96875, -0.876543),
    glm::vec2(-0.984375, -0.209877)
};

glm::mat4 Camera::ProjectionMatrix() const
{
    glm::mat4 proj;
    const auto windowSize{ Window::GetSize() };
    if (ProjectionType() == Camera::Projection::Perspective) {
        if (Zfar() > 0)
            proj = glm::perspective(glm::radians(Fov()), float(windowSize.x) / float(windowSize.y), Znear(), Zfar());
        else
            proj = glm::infinitePerspective(glm::radians(Fov()), float(windowSize.x) / float(windowSize.y), Znear());
    } else
        proj = glm::ortho(_frustum.x, _frustum.y, _frustum.z, _frustum.w, _znear, _zfar);
    auto halton{ haltonSequence[Render::FrameNumber() % 64] };
    proj[2][0] += halton.x * 0.25 / float(windowSize.x);
    proj[2][1] += halton.y * 0.25 / float(windowSize.y);
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
