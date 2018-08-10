/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:30:14 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/10 15:32:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>

class VertexArray;
class Shader;

namespace Render
{
	void		scene();
	void		present();
	void		bind_textures(Shader *shader);
	void		add_post_treatment(Shader *);
	void		remove_post_treatment(Shader *);
	const VertexArray	*display_quad();
	std::set<Shader*>	post_treatments;
};