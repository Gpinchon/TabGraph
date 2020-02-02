#pragma once

#include "Scene.hpp"
#include <memory>
#include <string> // for string
#include <vector>

namespace GLTF {
	std::vector<std::shared_ptr<Scene>> Parse(const std::string &path);
}