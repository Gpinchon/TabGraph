/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/16 12:40:44 by gpinchon         ###   ########.fr       */
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

t_framebuffer	shadow_buffer_build(t_engine *engine)
{
	t_framebuffer	f;
	
	ft_memset(&f, -1, sizeof(t_framebuffer));
	f.texture_depth = texture_create(engine, new_vec2(SHADOWRES, SHADOWRES), GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);
	texture_set_parameters(engine, f.texture_depth, 6,
		(GLenum[6]){GL_TEXTURE_COMPARE_FUNC, GL_TEXTURE_COMPARE_MODE, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[6]){GL_LEQUAL, GL_COMPARE_REF_TO_TEXTURE, GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP});
	glGenFramebuffers(1, &f.id);
	glBindFramebuffer(GL_FRAMEBUFFER, f.id);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_get_ogl_id(engine, f.texture_depth), 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return (f);
}

t_framebuffer	render_buffer_build(t_engine *e)
{
	t_framebuffer	f;

	f.texture_color = texture_create(e, new_vec2(IWIDTH, IHEIGHT), GL_TEXTURE_2D, GL_RGBA16F_ARB, GL_RGBA);
	texture_set_parameters(e, f.texture_color, 4, 
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP});
	f.texture_position = texture_create(e, new_vec2(IWIDTH, IHEIGHT), GL_TEXTURE_2D, GL_RGB32F_ARB, GL_RGB);
	texture_set_parameters(e, f.texture_position, 4,
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP});
	f.texture_normal = texture_create(e, new_vec2(IWIDTH, IHEIGHT), GL_TEXTURE_2D, GL_RGB16F_ARB, GL_RGB);
	texture_set_parameters(e, f.texture_normal, 4,
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP});
	f.texture_depth = texture_create(e, new_vec2(IWIDTH, IHEIGHT), GL_TEXTURE_2D, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
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
	printf("window %p\n", window->sdl_window);
	window->gl_context = SDL_GL_CreateContext(window->sdl_window);
	printf("context %p\n", window->gl_context);
	glewExperimental = GL_TRUE;
	if (!window->sdl_window || glewInit() != GLEW_OK)
	{
		free(window);
		return (window = NULL);
	}
	window->display_quad = create_display_quad();
	window->render_shader = load_shaders(engine, "render", "/src/shaders/render.vert", "/src/shaders/render.frag");
	window->render_buffer = render_buffer_build(engine);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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

void	callback_background(t_engine *engine, SDL_Event *event)
{
	static unsigned	background = 0;

	if (event && event->type == SDL_KEYDOWN)
		return;
	background = CYCLE(background + 1, 0, engine->textures_env.length / 2);
	engine->env = *((int*)ezarray_get_index(engine->textures_env, background * 2 + 0));
	engine->env_spec = *((int*)ezarray_get_index(engine->textures_env, background * 2 + 1));
	(void)event;
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
			phi += (event->key.keysym.sym == SDLK_DOWN ? 0.001 : -0.001) * engine->delta_time;
		else if (event->key.keysym.sym == SDLK_LEFT
		|| event->key.keysym.sym == SDLK_RIGHT)
			theta += (event->key.keysym.sym == SDLK_LEFT ? 0.001 : -0.001) * engine->delta_time;
		else if (event->key.keysym.sym == SDLK_KP_PLUS
		|| event->key.keysym.sym == SDLK_KP_MINUS)
			radius += (event->key.keysym.sym == SDLK_KP_PLUS ? -0.001 : 0.001) * engine->delta_time;
		else if (event->key.keysym.sym == SDLK_PAGEDOWN
		|| event->key.keysym.sym == SDLK_PAGEUP)
			((t_transform*)ezarray_get_index(engine->transforms,
			camera_get_target_index(engine, 0)))->position.y
			+= (event->key.keysym.sym == SDLK_PAGEUP ? 0.001 : -0.001) * engine->delta_time;
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
	return (0);
}

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

FRUSTUM	full_scene_frustum(t_engine *engine)
{
	t_mesh	*mesh;
	int		mesh_index = 0;
	VEC3	min = new_vec3(1000, 1000, 1000), max = new_vec3(-1000, -1000, -1000);

	while ((mesh = ezarray_get_index(engine->meshes, mesh_index)))
	{
		t_transform *t = ezarray_get_index(engine->transforms, mesh->transform_index);
		VEC3	curmin, curmax;
		curmin = mat4_mult_vec3(t->transform, mesh->bounding_box.min);
		curmax = mat4_mult_vec3(t->transform, mesh->bounding_box.max);
		min.x = curmin.x < min.x ? curmin.x : min.x;
		min.y = curmin.y < min.y ? curmin.y : min.y;
		min.z = curmin.z < min.z ? curmin.z : min.z;
		max.x = curmax.x > max.x ? curmax.x : max.x;
		max.y = curmax.y > max.y ? curmax.y : max.y;
		max.z = curmax.z > max.z ? curmax.z : max.z;
		//printf("curmin %f, %f, %f\n", curmin.x, curmin.y, curmin.z);
		//printf("curmax %f, %f, %f\n", curmax.x, curmax.y, curmax.z);
		mesh_index++;
	}
	//float minimum = MIN(min.x, MIN(min.y, min.z));
	//float maximum = MAX(max.x, MAX(max.y, max.z));
	//float value = MAX(fabs(minimum), fabs(maximum));
	float value = MAX(1.5, vec3_distance(min, max) / 2.f);
	return (new_frustum(-value, value, -value, value));
}

void	shadow_render(t_engine *engine)
{
	FRUSTUM frustum = full_scene_frustum(engine);
	MAT4	projection = mat4_orthographic(frustum, frustum.x, frustum.y);
	MAT4	view = mat4_lookat(new_vec3(-1, 1, 0), new_vec3(0, 0, 0), UP);
	MAT4	transform;
	t_mesh	*mesh;
	int		mesh_index = 0;
	t_light	*light = ezarray_get_index(engine->lights, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, light->framebuffer.id);
	glClearDepthf(1);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWRES, SHADOWRES);
	glEnable(GL_DEPTH_TEST);
	while ((mesh = ezarray_get_index(engine->meshes, mesh_index)))
	{
		t_vgroup	*vgroup;
		int			vgroup_index = 0;
		t_transform *t = ezarray_get_index(engine->transforms, mesh->transform_index);
		transform = mat4_combine(projection, view, t->transform);
		while ((vgroup = ezarray_get_index(mesh->vgroups, vgroup_index)))
		{
			t_material *material = ezarray_get_index(engine->materials, vgroup->mtl_index);
			shader_set_uniform(engine, material->shader_index,
				material->in_shadowtransform, &transform);
			shader_set_texture(engine, material->shader_index,
				material->in_texture_shadow, light->framebuffer.texture_depth, GL_TEXTURE8);
			shader_use(engine, light->shader_index);
			shader_set_texture(engine, light->shader_index,
				shader_get_uniform_index(engine, light->shader_index, "in_Texture_Albedo"),
				material->data.texture_albedo, GL_TEXTURE0);
			int use_texture = material->data.texture_albedo == -1 ? 0 : 1;
			shader_set_uniform(engine, light->shader_index,
				shader_get_uniform_index(engine, light->shader_index, "in_Use_Texture_Albedo"),
				&use_texture);
			shader_set_uniform(engine, light->shader_index,
				shader_get_uniform_index(engine, light->shader_index, "in_Transform"), &transform);
			glBindVertexArray(vgroup->v_arrayid);
			glDrawArrays(GL_TRIANGLES, 0, vgroup->v.length);
			glBindVertexArray(0);
			vgroup_index++;
		}
		mesh_index++;
	}
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
		ticks = SDL_GetTicks();
		engine->delta_time = ticks - last_ticks;
		SDL_PumpEvents();
		event_refresh(engine);
		glClear(engine->window->clear_mask);

		
		shadow_render(engine);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);

		glBindFramebuffer(GL_FRAMEBUFFER, engine->window->render_buffer.id);
		glViewport(0, 0, IWIDTH, IHEIGHT);
		scene_render(engine, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);
		shader_use(engine, engine->window->render_shader);
		glDisable(GL_DEPTH_TEST);
		texture_generate_mipmap(engine, engine->window->render_buffer.texture_color);
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

int	light_create(t_engine *engine, VEC3 position, VEC3 color, float power)
{
	t_light l;

	l.framebuffer = shadow_buffer_build(engine);
	l.shader_index = shader_get_by_name(engine, "shadow");
	l.data.directional.power = power;
	l.data.directional.color = color;
	l.transform_index = transform_create(engine, position, new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	ezarray_push(&engine->lights, &l);
	return (engine->lights.length - 1);
}

int main(int argc, char *argv[])
{
	t_engine	*e;

	if (argc < 2)
		return (0);
	e = engine_init();
	window_init(e, "Scope", WIDTH, HEIGHT);
	printf("%s\n", glGetString(GL_VERSION));
	printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	load_shaders(e, "default", "/src/shaders/default.vert", "/src/shaders/default.frag");
	load_shaders(e, "shadow", "/src/shaders/shadow.vert", "/src/shaders/shadow.frag");
	engine_load_env(e);
	light_create(e, new_vec3(-1, 1, 0), new_vec3(1, 1, 1), 1);
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
	engine_set_key_callback(e, SDL_SCANCODE_SPACE, callback_background);
	mesh_load(e, obj);
	main_loop(e);
	return (argc + argv[0][0]);
}