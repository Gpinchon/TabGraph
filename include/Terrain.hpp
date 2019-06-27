/*
* @Author: gpi
* @Date:   2019-03-26 12:03:23
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:36:57
*/

#pragma once

#include <Mesh.hpp>  // for Mesh
#include <memory>    // for shared_ptr
#include <string>    // for string
#include "glm"     // for glm::vec2, glm::vec3

class Texture;

class Terrain: public Mesh {
public:
	static std::shared_ptr<Terrain> create(const std::string& name, glm::vec2 resolution, glm::vec3 scale, std::shared_ptr<Texture> = nullptr);
	static std::shared_ptr<Terrain> create(const std::string& name, glm::vec2 resolution, const std::string &path);
private:
	Terrain(const std::string& name);
	std::shared_ptr<Texture>	_terrainData {nullptr};
	glm::vec2						_terrainResolution {0, 0};
	glm::vec3						_terrainSize {0, 0, 0};
};