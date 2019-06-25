/*
* @Author: gpi
* @Date:   2019-03-26 12:03:23
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 15:54:06
*/

#pragma once

#include <Mesh.hpp>  // for Mesh
#include <memory>    // for shared_ptr
#include <string>    // for string
#include "vml.h"     // for VEC2, VEC3

class Texture;

class Terrain: public Mesh {
public:
	static std::shared_ptr<Terrain> create(const std::string& name, VEC2 resolution, VEC3 scale, std::shared_ptr<Texture> = nullptr);
	static std::shared_ptr<Terrain> create(const std::string& name, VEC2 resolution, const std::string &path);
private:
	Terrain(const std::string& name);
	std::shared_ptr<Texture>	_terrainData {nullptr};
	VEC2						_terrainResolution {0, 0};
	VEC3						_terrainSize {0, 0, 0};
};