#pragma once

#include "Scene.hpp"
#include <string> // for string
#include <vector>

namespace GLTF {
	std::vector<Scene> Parse(const std::string &path);
}