/*
* @Author: gpi
* @Date:   2019-04-08 08:40:02
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-07 15:26:33
*/

#include "Engine.hpp"
#include "Texture.hpp"
#include "TextureParser.hpp"
#include "Debug.hpp"
#include "parser/TerrainData.hpp"

std::shared_ptr<Texture>  TerrainData::parse(const std::string& texture_name, const std::string& imagepath)
{
	return TextureParser::parse(texture_name, imagepath);
}
