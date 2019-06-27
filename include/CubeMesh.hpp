/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:59:34
*/

#pragma once

#include <memory>  		// for shared_ptr
#include <string>  		// for string
#include "glm/glm.hpp"  // for glm::vec3

class Mesh;

namespace CubeMesh {
	std::shared_ptr<Mesh> create(const std::string& name, glm::vec3 size);
};