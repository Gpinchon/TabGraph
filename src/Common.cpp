/*
* @Author: gpi
* @Date:   2019-07-15 09:09:13
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-15 09:12:43
*/

#include "Common.hpp"
#include <glm/vec3.hpp>

static glm::vec3 s_up(0, 1, 0);
static glm::vec3 s_forward(0, 0, -1);
static glm::vec3 s_right(1, 0, 0);

glm::vec3	Common::Up()
{
	return s_up;
}

void	Common::SetUp(glm::vec3 up)
{
	s_up = up;
}

glm::vec3	Common::Forward()
{
	return s_forward;
}

void	Common::SetForward(glm::vec3 up)
{
	s_forward = up;
}

glm::vec3	Common::Right()
{
	return s_right;
}

void	Common::SetRight(glm::vec3 up)
{
	s_right = up;
}
