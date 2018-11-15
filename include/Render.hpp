/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:30:14 by gpinchon          #+#    #+#             */
/*   Updated: 2018/11/15 21:17:22 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <set>

class VertexArray;
class Shader;

namespace Render
{
	void		update();
	void		fixed_update();
	void		scene();
	void		add_post_treatment(std::shared_ptr<Shader>);
	void		add_post_treatment(const std::string &name, const std::string &path);
	void		remove_post_treatment(std::shared_ptr<Shader>);
	const std::shared_ptr<VertexArray>	display_quad();
	std::vector<std::weak_ptr<Shader>>	post_treatments;
};