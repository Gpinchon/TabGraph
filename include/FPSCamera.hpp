/*
* @Author: gpi
* @Date:   2019-07-15 10:36:53
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-15 12:11:34
*/

#include "Camera.hpp"
#include <glm/vec3.hpp>

class FPSCamera : public Camera
{
public:
	static std::shared_ptr<FPSCamera> create(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);
	static std::shared_ptr<FPSCamera> current();
	float Yaw();
	void SetYaw(float);
	float Pitch();
	void SetPitch(float);
	float Roll();
	void SetRoll(float);
	glm::vec3 Forward() override;
	virtual void transform_update() override;

protected:
	FPSCamera(const std::string& name, float fov, CameraProjection proj = PerspectiveCamera);
	float _yaw {0};
	float _pitch {0};
	float _roll {0};
};