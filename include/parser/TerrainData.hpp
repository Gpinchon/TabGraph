#pragma once

#include <memory>

class Texture;

/*
** .TerrainData parsing interface
*/
namespace TerrainData {
std::shared_ptr<Texture> parse(const std::string& texture_name, const std::string& imagepath);
};