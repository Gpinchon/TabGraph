/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/08 13:44:10 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

/*
** quad is a singleton
*/

static float	*create_display_quad()
{
	static float	*quad = NULL;

	if (quad || !(quad = malloc(sizeof(float) * 8)))
		return (quad);
	quad[0] = -1.0f; quad[1] = -1.0f;
	quad[2] = 1.0f; quad[3] = -1.0f;
	quad[4] = -1.0f; quad[5] = 1.0f;
	quad[6] = 1.0f; quad[7] = 1.0f;
	return (quad);
}

t_framebuffer	framebuffer_build(t_engine *e)
{
	t_framebuffer	f;

	f.texture_color = texture_create(e, new_vec2(WIDTH, HEIGHT), GL_TEXTURE_2D, GL_RGBA16F_ARB, GL_RGBA);
	texture_set_parameters(e, f.texture_color, 4, 
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP});
	f.texture_position = texture_create(e, new_vec2(WIDTH, HEIGHT), GL_TEXTURE_2D, GL_RGB32F_ARB, GL_RGB);
	texture_set_parameters(e, f.texture_position, 4,
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP});
	f.texture_normal = texture_create(e, new_vec2(WIDTH, HEIGHT), GL_TEXTURE_2D, GL_RGB16F_ARB, GL_RGB);
	texture_set_parameters(e, f.texture_normal, 4,
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP});
	f.texture_depth = texture_create(e, new_vec2(WIDTH, HEIGHT), GL_TEXTURE_2D, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	texture_set_parameters(e, f.texture_depth, 4,
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP});
	glGenFramebuffers(1, &f.id);
	glBindFramebuffer(GL_FRAMEBUFFER, f.id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_get_ogl_id(e, f.texture_color), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture_get_ogl_id(e, f.texture_normal), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texture_get_ogl_id(e, f.texture_position), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_get_ogl_id(e, f.texture_depth), 0);
	glDrawBuffers(3, (GLenum[3]){GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return (f);
}

/*
** window is a singleton
*/
t_window		*window_init(t_engine *engine, const char *name, int width, int height)
{
	static t_window	*window = NULL;

	if (window || !(window = ft_memalloc(sizeof(t_window))))
		return (window);
	window->sdl_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	window->gl_context = SDL_GL_CreateContext(window->sdl_window);
	glewExperimental = GL_TRUE;
	if (!window->sdl_window || glewInit() != GLEW_OK)
	{
		free(window);
		return (window = NULL);
	}
	window->display_quad = create_display_quad();
	window->render_shader = load_shaders(engine, "render", "/src/shaders/render.vert", "/src/shaders/render.frag");
	window->render_buffer = framebuffer_build(engine);
	return (engine->window = window);
}

void	scene_render(t_engine *engine, int camera_index)
{
	unsigned	mesh_index;

	mesh_index = 0;
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera_update(engine, camera_index);
	while (mesh_index < engine->meshes.length)
	{
		mesh_render(engine, camera_index, mesh_index);
		mesh_index++;
	}
}

void	event_window(t_engine *engine, SDL_Event *event)
{
	if (event->window.event == SDL_WINDOWEVENT_CLOSE)
		engine->loop = 0;
}

void	callback_camera(t_engine *engine, SDL_Event *event)
{
	static float phi = M_PI / 2.f;
	static float theta = M_PI / 2.f;
	static float radius = 5.f;

	if (event && event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_UP
		|| event->key.keysym.sym == SDLK_DOWN)
			phi += (event->key.keysym.sym == SDLK_DOWN ? 0.1 : -0.1);
		else if (event->key.keysym.sym == SDLK_LEFT
		|| event->key.keysym.sym == SDLK_RIGHT)
			theta += (event->key.keysym.sym == SDLK_LEFT ? 0.1 : -0.1);
		else if (event->key.keysym.sym == SDLK_KP_PLUS
		|| event->key.keysym.sym == SDLK_KP_MINUS)
			radius += event->key.keysym.sym == SDLK_KP_PLUS ? -0.1 : 0.1;
		else if (event->key.keysym.sym == SDLK_PAGEDOWN
		|| event->key.keysym.sym == SDLK_PAGEUP)
			((t_transform*)ezarray_get_index(engine->transforms,
			camera_get_target_index(engine, 0)))->position.y
			+= event->key.keysym.sym == SDLK_PAGEUP ? 0.1 : -0.1;
		phi = CLAMP(phi, 0.01, M_PI - 0.01);
		theta = CYCLE(theta, 0, 2 * M_PI);
		radius = CLAMP(radius, 0.1f, 1000.f);
	}
	camera_orbite(engine, 0, phi, theta, radius);
}

void	event_keyboard(t_engine *engine, SDL_Event *event)
{
	if (engine->kcallbacks[event->key.keysym.scancode])
		engine->kcallbacks[event->key.keysym.scancode](engine, event);
}

int 	event_callback(void *e, SDL_Event *event)
{
	t_engine *engine;

	engine = e;
	if (event->type == SDL_QUIT)
		engine->loop = 0;
	else if (event->type == SDL_WINDOWEVENT)
		event_window(engine, event);
	else if (event->type == SDL_KEYUP
		|| event->type == SDL_KEYDOWN)
		event_keyboard(engine, event);
	return (0);
}

int		event_refresh(void *e)
{
	t_engine *engine;
	static VEC3	rotation = (VEC3){0, 0, 0};

	engine = e;
	rotation.y = CYCLE(rotation.y + 0.0001 * engine->delta_time, 0, 2 * M_PI);
	mesh_rotate(engine, 0, rotation);
	printf("\rFPS %i%c[2K", (int)(1000.f / (float)(engine->delta_time)), 27);
	return (0);
}

int	main_loop(t_engine *engine)
{
	unsigned ticks, last_ticks;

	SDL_GL_SetSwapInterval(engine->swap_interval);
	last_ticks = SDL_GetTicks();
	SDL_SetEventFilter(event_callback, engine);


	GLuint bufferid, render_quadid;
	glGenVertexArrays(1, &render_quadid);
	glBindVertexArray(render_quadid);
	glGenBuffers(1, &bufferid);
	glBindBuffer(GL_ARRAY_BUFFER, bufferid);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), engine->window->display_quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while(engine->loop)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, engine->window->render_buffer.id);
		ticks = SDL_GetTicks();
		engine->delta_time = ticks - last_ticks;
		SDL_PumpEvents();
		event_refresh(engine);
		glClear(engine->window->clear_mask);
		scene_render(engine, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		shader_use(engine, engine->window->render_shader);
		glDisable(GL_DEPTH_TEST);
		shader_set_texture(engine, engine->window->render_shader,
			shader_get_uniform_index(engine, engine->window->render_shader, "in_Texture_Color"),
			engine->window->render_buffer.texture_color, GL_TEXTURE0);
		shader_set_texture(engine, engine->window->render_shader,
			shader_get_uniform_index(engine, engine->window->render_shader, "in_Texture_Normal"),
			engine->window->render_buffer.texture_normal, GL_TEXTURE1);
		shader_set_texture(engine, engine->window->render_shader,
			shader_get_uniform_index(engine, engine->window->render_shader, "in_Texture_Position"),
			engine->window->render_buffer.texture_normal, GL_TEXTURE2);
		shader_set_texture(engine, engine->window->render_shader,
			shader_get_uniform_index(engine, engine->window->render_shader, "in_Texture_Depth"),
			engine->window->render_buffer.texture_depth, GL_TEXTURE3);
		shader_set_texture(engine, engine->window->render_shader,
			shader_get_uniform_index(engine, engine->window->render_shader, "in_Texture_Env"),
			engine->env, GL_TEXTURE4);
		t_camera *camera = ezarray_get_index(engine->cameras, 0);
		MAT4 matrix = mat4_inverse(camera->view);
		shader_set_uniform(engine, engine->window->render_shader,
			shader_get_uniform_index(engine, engine->window->render_shader, "in_InvViewMatrix"),
			&matrix);
		matrix = mat4_inverse(camera->projection);
		shader_set_uniform(engine, engine->window->render_shader,
			shader_get_uniform_index(engine, engine->window->render_shader, "in_InvProjMatrix"),
			&matrix);
		glBindVertexArray(render_quadid);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);
		SDL_GL_SwapWindow(engine->window->sdl_window);
		last_ticks = ticks;
	}
	return (0);
}

int main(int argc, char *argv[])
{
	t_engine	*e;

	if (argc < 2)
		return (0);
	e = engine_init();
	window_init(e, "Scope", WIDTH, HEIGHT);
	printf("%s\n", glGetString(GL_VERSION));
	load_shaders(e, "default", "/src/shaders/default.vert", "/src/shaders/default.frag");
	engine_load_env(e);
	int obj = load_obj(e, argv[1]);
	mesh_center(e, obj);
	int camera = camera_create(e, 45);
	camera_set_target(e, camera, transform_create(e, new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1)));
	camera_orbite(e, camera, M_PI / 2.f, M_PI / 2.f, 5.f);
	engine_set_key_callback(e, SDL_SCANCODE_UP, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_DOWN, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_LEFT, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_RIGHT, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_KP_PLUS, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_KP_MINUS, callback_camera);	
	engine_set_key_callback(e, SDL_SCANCODE_PAGEDOWN, callback_camera);	
	engine_set_key_callback(e, SDL_SCANCODE_PAGEUP, callback_camera);	
	mesh_load(e, obj);
	main_loop(e);
	return (argc + argv[0][0]);
}