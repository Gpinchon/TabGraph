/*
* @Author: gpi
* @Date:   2019-07-15 10:36:53
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-15 14:46:19
*/

#include "Camera.hpp"
#include <glm/vec3.hpp>

class FPSCamera : public Camera
{
public:
	static std::shared_ptr<FPSCamera> create(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);
	virtual float Yaw() const;
	virtual void SetYaw(float);
	virtual float Pitch() const;
	virtual void SetPitch(float);
	virtual float Roll() const;
	virtual void SetRoll(float);
	virtual glm::vec3 Forward() const override;
	virtual void UpdateViewMatrix() override;

protected:
	FPSCamera(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);
	float _yaw {0};
	float _pitch {0};
	float _roll {0};
};