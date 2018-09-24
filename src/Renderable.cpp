/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderable.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:57:06 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 17:28:51 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Renderable.hpp"
#include "Material.hpp"
#include "Texture.hpp"

std::vector<std::shared_ptr<Renderable>>	Renderable::_renderables;

Renderable::Renderable(const std::string &name) : Node(name) {}

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

bool						Renderable::is_loaded()
{
	return (_is_loaded);
}
