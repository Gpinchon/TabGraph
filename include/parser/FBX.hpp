/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-08-06 11:33:02
*/

#pragma once

#include <memory>
#include <string> // for string

class Mesh;

namespace FBX
{
std::shared_ptr<Mesh> parseMesh(const std::string &name, const std::string &path);
};
