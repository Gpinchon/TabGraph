/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:17
*/

#pragma once

#include "Assets/AssetsContainer.hpp"

#include <filesystem>
#include <map> // for map
#include <memory> // for shared_ptr
#include <string> // for string

class Material;

/*
** .MTLLIB parsing interface
*/
namespace MTLLIB {
std::shared_ptr<AssetsContainer> Parse(const std::filesystem::path path);
};
