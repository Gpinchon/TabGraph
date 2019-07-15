/*
* @Author: gpi
* @Date:   2019-07-15 10:36:36
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-15 12:10:33
*/

#include "FPSCamera.hpp"
#include <memory>

FPSCamera::FPSCamera(const std::string& name, float fov, CameraProjection proj) : Camera(name, fov, proj)
{
}

std::shared_ptr<FPSCamera> FPSCamera::create(const std::string& name, float ifov, CameraProjection proj)
{
    std::shared_ptr<FPSCamera> camera(new FPSCamera(name, ifov, proj));
    Node::add(camera);
    _cameras.push_back(camera);
    return (camera);
}

std::shared_ptr<FPSCamera> FPSCamera::current()
{
	return std::dynamic_pointer_cast<FPSCamera>(Camera::current());
}

float FPSCamera::Yaw()
{
	return _yaw;
}

void FPSCamera::SetYaw(float yaw)
{
	_yaw = yaw;
}

float FPSCamera::Pitch()
{
	return _pitch;
}

void FPSCamera::SetPitch(float pitch)
{
	_pitch = pitch;
}

float FPSCamera::Roll()
{
	return _roll;
}

void FPSCamera::SetRoll(float roll)
{
	_roll = roll;
}

glm::vec3 FPSCamera::Forward()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(Pitch())) * cos(glm::radians(Yaw()));
	direction.y = sin(glm::radians(Pitch()));
	direction.z = cos(glm::radians(Pitch())) * sin(glm::radians(Yaw()));
	return direction;
}

void FPSCamera::transform_update()
{
	//SetForward(direction);
	Camera::transform_update();
}