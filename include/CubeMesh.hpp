/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 15:58:13
*/

#pragma once

#include <memory>  // for shared_ptr
#include <string>  // for string
#include "vml.h"   // for VEC3
class Mesh;

namespace CubeMesh {
std::shared_ptr<Mesh> create(const std::string& name, VEC3 size);
};