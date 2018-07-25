/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cubemap.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/25 21:27:18 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Texture.hpp"
#include <array>

class Cubemap : public Texture
{
public:
	static Cubemap	*create(const std::string &);
	static Cubemap	*create(const std::string &, Texture *);
	static Cubemap	*parse(const std::string &, const std::string &);
	std::array<Texture *, 6>	sides{{nullptr}};
private:
	Cubemap(const std::string &);
};