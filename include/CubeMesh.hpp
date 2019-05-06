/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:28:33
*/

#pragma once

#include "Mesh.hpp"

namespace CubeMesh {
std::shared_ptr<Mesh> create(const std::string& name, VEC3 size);
};