/*
* @Author: gpi
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-16 13:55:48
*/

#include <OrbitCamera.hpp>

OrbitCamera::OrbitCamera(const std::string& iname, float ifov, float phi, float theta, float radius, CameraProjection proj)
    : Camera(iname, ifov, proj)
{
    _phi = phi;
    _theta = theta;
    _radius = radius;
}

std::shared_ptr<OrbitCamera> OrbitCamera::create(const std::string& iname, float ifov, float phi, float theta, float radius, CameraProjection proj)
{
    std::shared_ptr<OrbitCamera> camera(new OrbitCamera(iname, ifov, phi, theta, radius, proj));
    Node::add(camera);
    Camera::add(camera);
    return (camera);
}

void OrbitCamera::UpdateViewMatrix()
{
	glm::vec3 targetPosition(0);
    if (target() != nullptr)
        targetPosition = target()->Position();
    SetPosition(targetPosition + Radius() * glm::vec3(sin(Phi()) * cos(Theta()), sin(Phi()) * sin(Theta()), cos(Phi())));
    SetForward(Position() - targetPosition);
	SetViewMatrix(glm::lookAt(Position(), targetPosition, Common::Up()));
}

float OrbitCamera::Phi() const
{
	return _phi;
}

void OrbitCamera::SetPhi(float phi)
{
	_phi = phi;
}

float OrbitCamera::Theta() const
{
	return _theta;
}

void OrbitCamera::SetTheta(float theta)
{
	_theta = theta;
}

float OrbitCamera::Radius() const
{
	return _radius;
}

void OrbitCamera::SetRadius(float radius)
{
	_radius = radius;
}
