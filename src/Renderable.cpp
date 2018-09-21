/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderable.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:57:06 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 18:15:07 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Renderable.hpp"
#include "Material.hpp"
#include "Texture.hpp"

std::vector<std::shared_ptr<Renderable>>	Renderable::_renderables;

/*bool	alpha_compare(Renderable	*m, Renderable *m1)
{
	auto	mat = m->material;
	auto	mat1 = m1->material;
	if (m->parent == m1) {
		return (false);
	} if (m1->parent == m) {
		return (true);
	}
	if ((mat == nullptr) && (mat1 == nullptr)) {
		return (false);
	}
	if (mat == nullptr) {
		return (true);
	}
	if (mat1 == nullptr) {
		return (false);
	}
	return mat->alpha > mat1->alpha || ((mat->texture_albedo != nullptr) && (mat1->texture_albedo != nullptr) &&
		mat->texture_albedo->bpp() <= 24 && mat1->texture_albedo->bpp() >= 32);
}*/

/*void	Renderable::alpha_sort()
{
	Engine::sort(alpha_compare);
}*/

std::shared_ptr<Renderable>	Renderable::get(unsigned index)
{
	if (index >= _renderables.size())
		return (nullptr);
	return (_renderables.at(index));
}

std::shared_ptr<Renderable> Renderable::get_by_name(const std::string &name)
{
	std::hash<std::string>	hash_fn;
	auto					h = hash_fn(name);
	for (auto r : _renderables) {
		if (h == r->id())
			return (r);
	}
	return (nullptr);
}

void						Renderable::set_material(std::shared_ptr<Material> mtl)
{
	if (mtl != nullptr)
		_material = mtl;
	else
		_material.reset();
}

std::shared_ptr<Material>	Renderable::material()
{
	return (_material.lock());
}
