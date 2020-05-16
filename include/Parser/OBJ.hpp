/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 12:01:08
*/

#pragma once

#include <vector>
#include <memory> // for shared_ptr
#include <string> // for string

class Mesh;
class Scene;

/*
** .OBJ parsing interface
*/
namespace OBJ {
std::shared_ptr<Mesh> Parse(const std::string&, const std::string&);
std::vector<std::shared_ptr<Scene>> ParseScene(const std::string&);
};
