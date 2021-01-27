/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:21
*/

#pragma once

#include <memory> // for shared_ptr
#include <string> // for string

class Image;

/*
** .BMP parsing interface
*/
namespace BMP {
void parse(std::shared_ptr<Image>);
void save(std::shared_ptr<Image>, const std::string&);
};