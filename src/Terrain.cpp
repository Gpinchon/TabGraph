/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Terrain.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/25 23:08:57 by gpinchon          #+#    #+#             */
/*   Updated: 2019/03/27 23:23:33 by gpinchon         ###   ########.fr       */
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
	vg->v.resize(uint32_t(resolution.x * resolution.y));
	vg->vn.resize(uint32_t(resolution.x * resolution.y));
	for (auto x = 0.f; x < resolution.x; x++) {
		for (auto y = 0.f; y < resolution.y; y++) {
			auto v2 = new_vec2(x / resolution.x, y / resolution.y);
			auto z = 0.f;
			if (texture) {
				z = texture->sample(v2).x;
			}
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
			if (x < resolution.x && y < resolution.y) {
				vg->i.push_back(uint32_t(x + y * resolution.x));
				vg->i.push_back(uint32_t((x + 1) + (y + 1) * resolution.x));
				vg->i.push_back(uint32_t((x + 1) + y * resolution.x));
			}
		}
	}
	for (auto i = 0u; i < vg->i.size() - 3; i += 3)
	{
		auto i0 = vg->i.at(i + 0);
		auto i1 = vg->i.at(i + 1);
		auto i2 = vg->i.at(i + 2);
		auto v0 = vg->v.at(i0);
		auto v1 = vg->v.at(i1);
		auto v2 = vg->v.at(i2);
		auto &n0 = vg->vn.at(i0);
		auto &n1 = vg->vn.at(i1);
		auto &n2 = vg->vn.at(i2);

		auto U = vec3_sub(v1, v0);
		auto V = vec3_sub(v2, v0);
		VEC3 N;

		N.x = U.y * V.z - U.z * V.y;
		N.y = U.z * V.x - U.x * V.z;
		N.z = U.x * V.y - U.y * V.x;

		N = vec3_normalize(N);
		N = vec3_scale(N, 255);

		if (n0.x == 0 && n0.y == 0 && n0.z == 0) {
			n0.x = N.x;
			n0.y = N.y;
			n0.z = N.z;
		}
		else {
			n0.x = (n0.x + N.x) / 2.f;
			n0.y = (n0.y + N.y) / 2.f;
			n0.z = (n0.z + N.z) / 2.f;
		}
		if (n1.x == 0 && n1.y == 0 && n1.z == 0) {
			n1.x = N.x;
			n1.y = N.y;
			n1.z = N.z;
		}
		else {
			n1.x = (n1.x + N.x) / 2.f;
			n1.y = (n1.y + N.y) / 2.f;
			n1.z = (n1.z + N.z) / 2.f;
		}
		if (n2.x == 0 && n2.y == 0 && n2.z == 0) {
			n2.x = N.x;
			n2.y = N.y;
			n2.z = N.z;
		}
		else {
			n2.x = (n2.x + N.x) / 2.f;
			n2.y = (n2.y + N.y) / 2.f;
			n2.z = (n2.z + N.z) / 2.f;
		}
	}
	vg->set_material(Material::create("default_terrain"));
	terrain->add(vg);
	return terrain;
}
