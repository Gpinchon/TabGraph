/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Terrain.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/25 23:08:57 by gpinchon          #+#    #+#             */
/*   Updated: 2019/03/26 23:01:23 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Terrain.hpp"
#include "Vgroup.hpp"
#include "Texture.hpp"
#include "Material.hpp"

Terrain::Terrain(const std::string &name) : Mesh(name)
{

}

std::shared_ptr<Terrain> Terrain::create(const std::string& name,
	VEC2 resolution, VEC3 scale, std::shared_ptr<Texture> texture)
{
	std::cout << name << " create" << std::endl;
	auto terrain = std::shared_ptr<Terrain>(new Terrain(name));
    Mesh::add(terrain);
    Renderable::add(terrain);
    Node::add(terrain);
	auto vg = Vgroup::create(name + "vgroup");
	vg->v = std::vector<VEC3>(uint32_t(resolution.x * resolution.y));
	for (auto x = 0.f; x < resolution.x; x++) {
		for (auto y = 0.f; y < resolution.y; y++) {
			auto v2 = new_vec2(x / resolution.x, y / resolution.y);
			auto z = 0;
			if (texture)
				z = texture->sample(v2).x;
			auto &v3 = vg->v.at(uint32_t(x + y * resolution.x));
			v3 = new_vec3(v2.x * scale.x, z * scale.z, v2.y * scale.y);
		}
	}
	for (auto x = 0.f; x < resolution.x; x++) {
		for (auto y = 0.f; y < resolution.y; y++) {
			if (x < resolution.x && y < resolution.y) {
				vg->i.push_back(uint32_t(x + y * resolution.x));
				vg->i.push_back(uint32_t(x + (y + 1) * resolution.x));
				vg->i.push_back(uint32_t((x + 1) + (y + 1) * resolution.x));
			}
		}
	}
	vg->set_material(Material::create("default_terrain"));
	terrain->add(vg);
	return terrain;
}
