/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:16
*/

#pragma once

#include "Assets/AssetsContainer.hpp"

#include <filesystem>
#include <memory> // for shared_ptr
#include <string> // for string
#include <vector>

class Mesh;
class Scene;

/*
** .OBJ parsing interface
*/
namespace OBJ {
std::shared_ptr<AssetsContainer> Parse(const std::filesystem::path path);
std::vector<std::shared_ptr<Scene>> ParseScene(const std::filesystem::path);
};
