/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:26:18
*/

#pragma once

#include <memory>

class Texture;

/*
** .BMP parsing interface
*/
namespace BMP {
std::shared_ptr<Texture> parse(const std::string& texture_name, const std::string& imagepath);
void save(std::shared_ptr<Texture>, const std::string&);
};