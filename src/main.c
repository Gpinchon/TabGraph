/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/15 16:11:26 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

/*
** quad is a singleton
*/

static GLuint	display_quad_get()
{
	static GLuint	render_quadid = 0;
	GLuint			bufferid;
	float			quad[8];

	if (render_quadid)
		return (render_quadid);
	quad[0] = -1.0f; quad[1] = -1.0f;
	quad[2] = 1.0f; quad[3] = -1.0f;
	quad[4] = -1.0f; quad[5] = 1.0f;
	quad[6] = 1.0f; quad[7] = 1.0f;
	glGenVertexArrays(1, &render_quadid);
	glBindVertexArray(render_quadid);
	glGenBuffers(1, &bufferid);
	glBindBuffer(GL_ARRAY_BUFFER, bufferid);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return (render_quadid);
}

void	scene_render(int camera_index)
{
	unsigned	mesh_index;

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera_update(camera_index);
	mesh_index = 0;
	while (mesh_index < engine_get()->meshes.length)
	{
		mesh_update(mesh_index);
		mesh_render(mesh_index, camera_index, render_opaque);
		mesh_index++;
	}
	mesh_index = 0;
	while (mesh_index < engine_get()->meshes.length)
	{
		mesh_sort_draw(mesh_index, camera_index);
		mesh_render(mesh_index, camera_index, render_transparent);
		mesh_index++;
	}
}

void	event_window(SDL_Event *event)
{
	if (event->window.event == SDL_WINDOWEVENT_CLOSE)
		engine_get()->loop = 0;
}

void	event_keyboard(SDL_Event *event)
{
	if (engine_get()->kcallbacks[event->key.keysym.scancode])
		engine_get()->kcallbacks[event->key.keysym.scancode](event);
}

int 	event_callback(void *e, SDL_Event *event)
{
	t_engine *engine;

	engine = e;
	if (event->type == SDL_QUIT)
		engine->loop = 0;
	else if (event->type == SDL_WINDOWEVENT)
		event_window(event);
	else if (event->type == SDL_KEYUP
		|| event->type == SDL_KEYDOWN)
		event_keyboard(event);
	return (0);
}

int		event_refresh()
{
	t_material		*m;
	int				i;
	static VEC3	rotation = (VEC3){0, 0, 0};

	static float val = 0;
	if (engine_get()->stupidity != engine_get()->new_stupidity)
	{
		val = CLAMP(val + 0.1, 0, 1);
		engine_get()->stupidity = interp_cubic(!engine_get()->new_stupidity, engine_get()->new_stupidity, val);
		i = 0;
		while ((m = ezarray_get_index(engine_get()->materials, i)))
		{
			m->data.stupidity = engine_get()->stupidity;
			i++;
		}
	}
	else
		val = 0;
	rotation.y = CYCLE(rotation.y + 0.1 * engine_get()->delta_time, 0, 2 * M_PI);
	callback_camera(NULL);
	mesh_rotate(0, rotation);
	return (0);
}

FRUSTUM	full_scene_frustum()
{
	t_mesh		*mesh;
	t_transform	*t;
	float		value;
	int			mesh_index = 0;
	VEC3		v[2];

	v[0] = new_vec3(1000, 1000, 1000);
	v[1] = new_vec3(-1000, -1000, -1000);
	while ((mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
	{
		t = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
		VEC3	curmin, curmax;
		curmin = mat4_mult_vec3(t->transform, mesh->bounding_box.min);
		curmax = mat4_mult_vec3(t->transform, mesh->bounding_box.max);
		v[0].x = curmin.x < v[0].x ? curmin.x : v[0].x;
		v[0].y = curmin.y < v[0].y ? curmin.y : v[0].y;
		v[0].z = curmin.z < v[0].z ? curmin.z : v[0].z;
		v[1].x = curmax.x > v[1].x ? curmax.x : v[1].x;
		v[1].y = curmax.y > v[1].y ? curmax.y : v[1].y;
		v[1].z = curmax.z > v[1].z ? curmax.z : v[1].z;
		mesh_index++;
	}
	value = MAX(1.5, vec3_distance(v[0], v[1]) / 2.f);
	return (new_frustum(-value, value, -value, value));
}

void	shadow_render()
{
	FRUSTUM frustum = full_scene_frustum();
	MAT4	projection = mat4_orthographic(frustum, frustum.x * 1.5f, frustum.y * 1.5f);
	MAT4	view = mat4_lookat(new_vec3(-1, 1, 0), new_vec3(0, 0, 0), UP);
	MAT4	transform;
	t_mesh	*mesh;
	int		mesh_index = 0;
	t_light	*light = ezarray_get_index(engine_get()->lights, 0);

	framebuffer_bind(light->render_buffer);
	glClearDepthf(1);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	while ((mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
	{
		t_vgroup	*vgroup;
		int			vgroup_index = 0;
		t_transform *t = ezarray_get_index(engine_get()->transforms, mesh->transform_index);
		transform = mat4_combine(projection, view, t->transform);
		while ((vgroup = ezarray_get_index(mesh->vgroups, vgroup_index)))
		{
			t_material *material = ezarray_get_index(engine_get()->materials, vgroup->mtl_index);
			shader_set_uniform(material->shader_index,
				material->in_shadowtransform, &transform);
			shader_bind_texture(material->shader_index,
				material->in_texture_shadow, framebuffer_get_depth(light->render_buffer), GL_TEXTURE20);
			if (material->data.alpha > 0.5f)
			{
				shader_use(framebuffer_get_shader(light->render_buffer));
				shader_bind_texture(framebuffer_get_shader(light->render_buffer),
					shader_get_uniform_index(framebuffer_get_shader(light->render_buffer), "in_Texture_Albedo"),
					material->data.texture_albedo, GL_TEXTURE0);
				int use_texture = material->data.texture_albedo == -1 ? 0 : 1;
				shader_set_uniform(framebuffer_get_shader(light->render_buffer),
					shader_get_uniform_index(framebuffer_get_shader(light->render_buffer), "in_Use_Texture_Albedo"),
					&use_texture);
				shader_set_uniform(framebuffer_get_shader(light->render_buffer),
					shader_get_uniform_index(framebuffer_get_shader(light->render_buffer), "in_Transform"), &transform);
				glBindVertexArray(vgroup->v_arrayid);
				glDrawArrays(GL_TRIANGLES, 0, vgroup->v.length);
				glBindVertexArray(0);
			}
			vgroup_index++;
		}
		mesh_index++;
	}
}

static inline void	blur_texture(int texture, int pass, float radius)
{
	static int				blur = -1;
	float					angle;
	VEC2					direction;

	if (blur == -1)
		blur = framebuffer_create(new_vec2(IWIDTH, IHEIGHT), shader_get_by_name("blur"), 1, 0);
	framebuffer_bind(blur);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	shader_use(framebuffer_get_shader(blur));
	glBindVertexArray(display_quad_get());
	angle = 0;
	pass *= 4;
	int	color0 = framebuffer_get_attachement(blur, 0);
	while (pass)
	{
		direction = vec2_scale(direction, radius);
		shader_bind_texture(framebuffer_get_shader(blur),
			shader_get_uniform_index(framebuffer_get_shader(blur), "in_Texture_Color"),
			texture, GL_TEXTURE0);
		shader_set_uniform(framebuffer_get_shader(blur),
			shader_get_uniform_index(framebuffer_get_shader(blur), "in_Direction"),
			&direction);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			texture_get_glid(color0), 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		angle = CYCLE(angle + 90, 0, 270);
		direction = mat2_mult_vec2(mat2_rotation(angle), new_vec2(1, 1));
		int temp = texture;
		texture = color0;
		color0 = temp;
		pass--;
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			texture_get_glid(framebuffer_get_attachement(blur, 0)), 0);
	shader_use(-1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void	render_present()
{
	int w, h;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	SDL_GetWindowSize(window_get()->sdl_window, &w, &h);
	glViewport(0, 0, w, h);
	int	shader = framebuffer_get_shader(window_get()->render_buffer);
	texture_generate_mipmap(framebuffer_get_attachement(window_get()->render_buffer, 0));
	shader_use(shader);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Color"),
		framebuffer_get_attachement(window_get()->render_buffer, 0), GL_TEXTURE0);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Bright"),
		framebuffer_get_attachement(window_get()->render_buffer, 1), GL_TEXTURE1);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Normal"),
		framebuffer_get_attachement(window_get()->render_buffer, 2), GL_TEXTURE2);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Position"),
		framebuffer_get_attachement(window_get()->render_buffer, 3), GL_TEXTURE3);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Depth"),
		framebuffer_get_depth(window_get()->render_buffer), GL_TEXTURE4);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Env"),
		engine_get()->env, GL_TEXTURE5);
	t_camera *camera = ezarray_get_index(engine_get()->cameras, 0);
	MAT4 matrix = mat4_inverse(camera->view);
	shader_set_uniform(shader,
		shader_get_uniform_index(shader, "in_InvViewMatrix"),
		&matrix);
	matrix = mat4_inverse(camera->projection);
	shader_set_uniform(shader,
		shader_get_uniform_index(shader, "in_InvProjMatrix"),
		&matrix);
	glBindVertexArray(display_quad_get());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
}

int	main_loop()
{
	float ticks, last_ticks;

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
		shadow_render();
		framebuffer_bind(window_get()->render_buffer);
		scene_render(0);
		blur_texture(framebuffer_get_attachement(window_get()->render_buffer, 1), BLOOMPASS, 10);
		blur_texture(framebuffer_get_attachement(window_get()->render_buffer, 2), 1, 5);
		render_present();
		SDL_GL_SwapWindow(window_get()->sdl_window);
	}
	return (0);
}

int	light_create(VEC3 position, VEC3 color, float power)
{
	t_light l;

	l.render_buffer = framebuffer_create(new_vec2(SHADOWRES, SHADOWRES), shader_get_by_name("shadow"), 0, 1);
	texture_set_parameters(framebuffer_get_depth(l.render_buffer), 6,
		(GLenum[6]){GL_TEXTURE_COMPARE_FUNC, GL_TEXTURE_COMPARE_MODE, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[6]){GL_LEQUAL, GL_COMPARE_REF_TO_TEXTURE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE});
	l.data.directional.power = power;
	l.data.directional.color = color;
	l.transform_index = transform_create(position, new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	ezarray_push(&engine_get()->lights, &l);
	return (engine_get()->lights.length - 1);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return (0);
	engine_init();
	window_init("Scope", WIDTH, HEIGHT);
	load_bmp("./res/stupid.bmp");
	load_shaders("render", "/src/shaders/render.vert", "/src/shaders/render.frag");
	load_shaders("default", "/src/shaders/default.vert", "/src/shaders/default.frag");
	load_shaders("shadow", "/src/shaders/shadow.vert", "/src/shaders/shadow.frag");
	load_shaders("blur", "/src/shaders/blur.vert", "/src/shaders/blur.frag");
	engine_load_env();
	light_create(new_vec3(-1, 1, 0), new_vec3(1, 1, 1), 1);
	int obj = load_obj(argv[1]);
	mesh_center(obj);
	int camera = camera_create(45);
	camera_set_target(camera, transform_create(new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1)));
	camera_orbite(camera, M_PI / 2.f, M_PI / 2.f, 5.f);
	engine_set_key_callback(SDL_SCANCODE_KP_PLUS, callback_scale);
	engine_set_key_callback(SDL_SCANCODE_KP_MINUS, callback_scale);
	engine_set_key_callback(SDL_SCANCODE_SPACE, callback_background);
	engine_set_key_callback(SDL_SCANCODE_ESCAPE, callback_exit);
	engine_set_key_callback(SDL_SCANCODE_RETURN, callback_fullscreen);
	engine_set_key_callback(SDL_SCANCODE_S, callback_stupidity);
	mesh_load(obj);
	main_loop();
	SDL_Quit();
	engine_destroy();
	return (argc + argv[0][0]);
}