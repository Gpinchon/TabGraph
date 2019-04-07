/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Terrain.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/25 22:56:11 by gpinchon          #+#    #+#             */
/*   Updated: 2019/04/07 22:29:36 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <Mesh.hpp>

class Texture;

class Terrain: public Mesh {
public:
	static std::shared_ptr<Terrain> create(const std::string& name, VEC2 resolution, VEC3 scale, std::shared_ptr<Texture> = nullptr);
	static std::shared_ptr<Terrain> create(const std::string& name, VEC2 resolution, const std::string &path);
private:
	Terrain(const std::string& name);
};