/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 13:45:28
*/

#pragma once

#include "Assets/AssetsContainer.hpp"

#include <memory> // for shared_ptr
#include <string> // for string
#include <vector>
#include <filesystem>

class Mesh;
class Scene;

/*
** .OBJ parsing interface
*/
namespace OBJ {
AssetsContainer Parse(const std::filesystem::path path);
std::vector<std::shared_ptr<Scene>> ParseScene(const std::filesystem::path);
};
