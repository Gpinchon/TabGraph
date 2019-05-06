/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:25:59
*/

#pragma once

#include "Material.hpp"

/*
** .MTLLIB parsing interface
*/
namespace MTLLIB {
bool parse(const std::string& path);
};
