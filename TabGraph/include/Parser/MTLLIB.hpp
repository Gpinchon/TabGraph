/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 12:00:13
*/

#pragma once

#include "Assets/AssetsContainer.hpp"

#include <string> // for string
#include <map> // for map
#include <memory> // for shared_ptr
#include <filesystem>

class Material;

/*
** .MTLLIB parsing interface
*/
namespace MTLLIB {
	std::shared_ptr<AssetsContainer> Parse(const std::filesystem::path path);
};
