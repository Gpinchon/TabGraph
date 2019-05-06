/*
* @Author: gpi
* @Date:   2019-04-08 08:40:02
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:25:29
*/

#pragma once

#include <memory>

class Texture;

/*
** .TerrainData parsing interface
*/
namespace TerrainData {
std::shared_ptr<Texture> parse(const std::string& texture_name, const std::string& imagepath);
};