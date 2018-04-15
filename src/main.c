/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/15 20:41:34 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

void	main_loop()
{
	float	ticks;
	float	last_ticks;

	SDL_GL_SetSwapInterval(engine_get()->swap_interval);
	last_ticks = SDL_GetTicks() / 1000.f;
	SDL_SetEventFilter(event_callback, engine_get());
	while(engine_get()->loop)
	{
		ticks = SDL_GetTicks() / 1000.f;
		engine_get()->delta_time = ticks - last_ticks;
		last_ticks = ticks;
		SDL_PumpEvents();
		event_refresh();
		glClear(window_get()->clear_mask);
		render_shadow();
		framebuffer_bind(window_get()->render_buffer);
		render_scene(0);
		texture_blur(framebuffer_get_attachement(window_get()->render_buffer, 1), BLOOMPASS, 2.5);
		texture_blur(framebuffer_get_attachement(window_get()->render_buffer, 2), 1, 2.5);
		render_present(0);
		SDL_GL_SwapWindow(window_get()->sdl_window);
	}
}

int	light_create(VEC3 position, VEC3 color, float power)
{
	t_light l;

	l.render_buffer = framebuffer_create(new_vec2(SHADOWRES, SHADOWRES), shader_get_by_name("shadow"), 0, 1);
	texture_set_parameters(framebuffer_get_depth(l.render_buffer), 2,
		(GLenum[2]){GL_TEXTURE_COMPARE_FUNC, GL_TEXTURE_COMPARE_MODE},
		(GLenum[2]){GL_LEQUAL, GL_COMPARE_REF_TO_TEXTURE});
	l.data.directional.power = power;
	l.data.directional.color = color;
	l.transform_index = transform_create(position, new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	ezarray_push(&engine_get()->lights, &l);
	return (engine_get()->lights.length - 1);
}

void	setup_callbacks()
{
	set_key_callback(SDL_SCANCODE_KP_PLUS, callback_scale);
	set_key_callback(SDL_SCANCODE_KP_MINUS, callback_scale);
	set_key_callback(SDL_SCANCODE_SPACE, callback_background);
	set_key_callback(SDL_SCANCODE_ESCAPE, callback_exit);
	set_key_callback(SDL_SCANCODE_RETURN, callback_fullscreen);
	set_key_callback(SDL_SCANCODE_S, callback_stupidity);
	set_key_callback(SDL_SCANCODE_Q, callback_quality);
	set_refresh_callback(callback_refresh);
}

int	main(int argc, char *argv[])
{
	int	camera;
	int obj;

	obj = -1;
	engine_init(argv[0]);
	window_init("Scop", WIDTH, HEIGHT);
	engine_load_env();
	light_create(new_vec3(-1, 1, 0), new_vec3(1, 1, 1), 1);
	camera = camera_create(45);
	camera_set_target(camera, transform_create(
		new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1)));
	camera_orbite(camera, M_PI / 2.f, M_PI / 2.f, 5.f);
	if (argc >= 2)
		obj = load_obj(argv[1]);
	if (argc > 2 || obj == -1)
		obj = load_obj("./res/obj/chart.obj");
	mesh_center(obj);
	mesh_load(obj);
	setup_callbacks();
	main_loop();
	SDL_Quit();
	engine_destroy();
	return (0);
}