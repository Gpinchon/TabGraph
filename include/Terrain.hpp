/*
* @Author: gpi
* @Date:   2019-03-26 12:03:23
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-10 14:20:06
*/

#pragma once

#include <Mesh.hpp>

class Texture;

class Terrain: public Mesh {
public:
	static std::shared_ptr<Terrain> create(const std::string& name, VEC2 resolution, VEC3 scale, std::shared_ptr<Texture> = nullptr);
	static std::shared_ptr<Terrain> create(const std::string& name, VEC2 resolution, const std::string &path);
private:
	Terrain(const std::string& name);
	std::shared_ptr<Texture>	_terrainData;
	VEC2						_terrainResolution;
	VEC3						_terrainSize;
};