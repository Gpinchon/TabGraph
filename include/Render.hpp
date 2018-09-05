/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:30:14 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/05 17:29:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <set>

class VertexArray;
class Shader;

namespace Render
{
	void		scene();
	void		add_post_treatment(Shader *);
	void		add_post_treatment(const std::string &name, const std::string &path);
	void		remove_post_treatment(Shader *);
	const VertexArray	*display_quad();
	std::set<Shader*>	post_treatments;
};