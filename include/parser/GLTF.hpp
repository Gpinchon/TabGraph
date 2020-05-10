#pragma once

#include <memory>
#include <string> // for string
#include <vector>

class Scene;

namespace GLTF {
	std::vector<std::shared_ptr<Scene>> Parse(const std::string &path);
}