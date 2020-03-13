/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 10:32:59
*/

#pragma once

#include <memory> // for shared_ptr
#include <string> // for string

class Texture2D;

/*
** .BMP parsing interface
*/
namespace BMP {
std::shared_ptr<Texture2D> parse(const std::string& texture_name, const std::string& imagepath);
void save(std::shared_ptr<Texture2D>, const std::string&);
};