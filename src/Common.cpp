/*
* @Author: gpi
* @Date:   2019-07-15 09:09:13
* @Last Modified by:   gpi
* @Last Modified time: 2019-07-15 09:12:43
*/

#include "Common.hpp"
#include <glm/vec3.hpp>

glm::vec3	&Common::Up() {
	static glm::vec3 s_up(0, 1, 0);
	return s_up;
}

void	Common::SetUp(glm::vec3 up) {
	Up() = up;
}
