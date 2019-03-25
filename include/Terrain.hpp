/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Terrain.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/25 22:56:11 by gpinchon          #+#    #+#             */
/*   Updated: 2019/03/25 23:08:24 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <Renderable.hpp>

class Texture;

class Terrain: public Renderable {
public:
	static std::shared_ptr<Renderable> create(const std::string& name, VEC2 resolution, VEC3 scale, std::shared_ptr<Texture> = nullptr);

private:
	Terrain(const std::string& name);
	//~Terrain();
};