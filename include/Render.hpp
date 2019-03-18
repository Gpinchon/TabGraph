/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:30:14 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/22 19:32:14 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <memory>
#include <string>
#include <vector>

class VertexArray;
class Shader;

class Render {
	public :
		static void update();
		static void fixed_update();
		static void scene();
		static void add_post_treatment(std::shared_ptr<Shader>);
		static void add_post_treatment(const std::string& name, const std::string& path);
		static void remove_post_treatment(std::shared_ptr<Shader>);
		static double delta_time();
		static const std::shared_ptr<VertexArray> display_quad();
		static std::vector<std::weak_ptr<Shader>>& post_treatments();

	private:
		double _delta_time{0};
};