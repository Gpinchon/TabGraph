/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 17:40:19
*/

#pragma once

#include "Assets/AssetsContainer.hpp"

#include <memory>
#include <string> // for string
#include <vector>
#include <filesystem>

class Scene;

namespace FBX {
	std::shared_ptr<AssetsContainer> Parse(const std::filesystem::path path);
};
