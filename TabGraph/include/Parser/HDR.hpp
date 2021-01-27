/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:18
*/

#pragma once

#include <memory> // for shared_ptr
#include <string> // for string

class Image;

/*
** .HDR parsing interface
*/
namespace HDR {
void Parse(const std::shared_ptr<Image> &);
};