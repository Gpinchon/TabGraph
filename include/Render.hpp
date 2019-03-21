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
#include <thread>

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
		static void start_rendering_thread();
		static void stop_rendering_thread();
		static void request_redraw();
		static double delta_time();
		static bool needs_update();
		static uint64_t frame_nbr(void);
		static const std::shared_ptr<VertexArray> display_quad();
		static std::vector<std::weak_ptr<Shader>>& post_treatments();

	private:
		static void _thread();
		static Render &_get();
		static Render *_instance;
		double _delta_time {0};
		bool _needs_update {true};
		bool _loop {true};
		uint64_t _frame_nbr{ 0 };
		std::thread _rendering_thread;
};