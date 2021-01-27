/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:20
*/

#pragma once

#include "Assets/AssetsContainer.hpp"

#include <filesystem>
#include <memory>
#include <string> // for string
#include <vector>

class Scene;

namespace FBX {
std::shared_ptr<AssetsContainer> Parse(const std::filesystem::path path);
};
