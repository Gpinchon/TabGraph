/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 11:59:01
*/

#pragma once

#include <memory>  // for shared_ptr
#include <string>  // for string

class Texture;

/*
** .HDR parsing interface
*/
namespace HDR {
std::shared_ptr<Texture> parse(const std::string& texture_name, const std::string& imagepath);
};