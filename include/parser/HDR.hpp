/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:26:06
*/

#pragma once

#include <memory>

class Texture;

/*
** .HDR parsing interface
*/
namespace HDR {
std::shared_ptr<Texture> parse(const std::string& texture_name, const std::string& imagepath);
};