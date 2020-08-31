/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 17:07:39
*/
#pragma once

#include "Assets/AssetsContainer.hpp"

#include <memory>
#include <string> // for string
#include <vector>
#include <filesystem>

namespace GLTF {
AssetsContainer Parse(const std::filesystem::path path);
}