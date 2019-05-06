/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:25:55
*/

#pragma once

#include <memory>

class Mesh;

/*
** .OBJ parsing interface
*/
namespace OBJ {
std::shared_ptr<Mesh> parse(const std::string&, const std::string&);
};
