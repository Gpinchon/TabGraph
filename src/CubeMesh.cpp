/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CubeMesh.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/19 22:58:50 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 15:39:07 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "CubeMesh.hpp"
#include "Vgroup.hpp"
#include "Material.hpp"

std::shared_ptr<Mesh>	CubeMesh::create(const std::string &name, VEC3 size)
{
	auto	m = Mesh::create(name);
	static std::vector<VEC3> cubeVertices {
		{-0.50f, -0.50f, 0.50f}, // back 
		{0.50f, -0.50f, 0.50f},
		{0.50f, 0.50f, 0.50f},
		{-0.50f, 0.50f, 0.50f},

		{-0.50f, -0.50f, -0.50f}, // front 
		{0.50f, -0.50f, -0.50f},
		{0.50f, 0.50f, -0.50f},
		{-0.50f, 0.50f, -0.50f},

		{-0.50f, -0.50f, -0.50f}, // left 
		{-0.50f, -0.50f, 0.50f},
		{-0.50f, 0.50f, 0.50f},
		{-0.50f, 0.50f, -0.50f},

		{0.50f, -0.50f, -0.50f}, // right 
		{0.50f, -0.50f, 0.50f},
		{0.50f, 0.50f, 0.50f},
		{0.50f, 0.50f, -0.50f},

		{-0.50f, 0.50f, 0.50f}, // top 
		{0.50f, 0.50f, 0.50f},
		{0.50f, 0.50f, -0.50f},
		{-0.50f, 0.50f, -0.50f},

		{-0.50f, -0.50f, 0.50f}, // bottom 
		{0.50f, -0.50f, 0.50f},
		{0.50f, -0.50f, -0.50f},
		{-0.50f, -0.50f, -0.50f}
	}; 
	static std::vector<CVEC4> cubeNormals{
		{127, 127, 255, 255},
		{127, 127, 255, 255},
		{127, 127, 255, 255},
		{127, 127, 255, 255},

		{127, 127, 0, 255},
		{127, 127, 0, 255},
		{127, 127, 0, 255},
		{127, 127, 0, 255}, 

		{0, 127, 127, 255},
		{0, 127, 127, 255},
		{0, 127, 127, 255},
		{0, 127, 127, 255}, 

		{255, 127, 127, 255},
		{255, 127, 127, 255},
		{255, 127, 127, 255},
		{255, 127, 127, 255}, 

		{127, 255, 127, 255},
		{127, 255, 127, 255},
		{127, 255, 127, 255},
		{127, 255, 127, 255}, 

		{127, 0, 127, 255},
		{127, 0, 127, 255},
		{127, 0, 127, 255},
		{127, 0, 127, 255}
	}; 
	static std::vector<VEC2> cubeTexCoords{
		{0, 0}, {1, 0}, {1, 1}, {0, 1}, 
		{1, 0}, {0, 0}, {0, 1}, {1, 1}, 
		{0, 0}, {1, 0}, {1, 1}, {0, 1}, 
		{1, 0}, {0, 0}, {0, 1}, {1, 1}, 
		{0, 0}, {1, 0}, {1, 1}, {0, 1}, 
		{1, 0}, {0, 0}, {0, 1}, {1, 1}
	};
	static std::vector<unsigned> cubeIndices{
		0, 1, 2, 0, 2, 3, // front 
		4, 6, 5, 4, 7, 6, // back 
		8, 9, 10, 8, 10, 11, // left 
		12, 14, 13, 12, 15, 14, // right 
		16, 17, 18, 16, 18, 19, // top 
		20, 22, 21, 20, 23, 22  // bottom
	};
	auto	vg = Vgroup::create(m->name() + "_vgroup");
	auto	thisCubeVertices = cubeVertices;
	for (auto &v : thisCubeVertices) {
		v = vec3_mult(v, size);
	}
	vg->set_material(Material::create(vg->name() + "_material"));
	vg->v = thisCubeVertices;
	vg->vn = cubeNormals;
	vg->vt = cubeTexCoords;
	vg->i = cubeIndices;
	m->add(vg);
	return (m);
}
