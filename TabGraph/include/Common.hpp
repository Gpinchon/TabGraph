/*
* @Author: gpi
* @Date:   2019-07-15 09:11:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-15 09:12:26
*/

#pragma once

#include <glm/vec3.hpp>

namespace Common
{
	glm::vec3 Up();
	void SetUp(glm::vec3);
	glm::vec3 Forward();
	void SetForward(glm::vec3);
	glm::vec3 Right();
	void SetRight(glm::vec3);
	/** Default gravity is { 0, -9.81, 0 } */
	glm::vec3 Gravity();
	void SetGravity(glm::vec3);
}