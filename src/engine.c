/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/25 22:28:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <dirent.h>

t_engine *g_engine = NULL;

/*
** engine is a singleton
*/

inline t_engine	*engine_get(void)
{
	if (!g_engine)
		g_engine = ft_memalloc(sizeof(t_engine));
	return (g_engine);
}

void			engine_init(void)
{
	t_engine	*engine;

	engine = engine_get();
	if (!engine)
		return ;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, MSAA);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE);
	engine->cameras = new_ezarray(other, 0, sizeof(t_camera));
	engine->shaders = new_ezarray(other, 0, sizeof(t_shader));
	engine->textures = new_ezarray(other, 0, sizeof(t_texture));
	engine->textures_env = new_ezarray(signed_short, 0);
	engine->materials = new_ezarray(other, 0, sizeof(t_material));
	engine->meshes = new_ezarray(other, 0, sizeof(t_mesh));
	engine->transforms = new_ezarray(other, 0, sizeof(t_transform));
	engine->lights = new_ezarray(other, 0, sizeof(t_light));
	engine->framebuffers = new_ezarray(other, 0, sizeof(t_framebuffer));
	engine->loop = 1;
	engine->swap_interval = 1;
	engine->program_path = convert_backslash(getcwd(NULL, 4096));
}

void			engine_destroy(void)
{
	t_engine *engine;

	engine = engine_get();
	destroy_ezarray(&engine->cameras);
	destroy_ezarray(&engine->shaders);
	destroy_ezarray(&engine->textures);
	destroy_ezarray(&engine->textures_env);
	destroy_ezarray(&engine->materials);
	destroy_ezarray(&engine->meshes);
	destroy_ezarray(&engine->transforms);
	destroy_ezarray(&engine->lights);
	destroy_ezarray(&engine->framebuffers);
	free(engine);
	g_engine = NULL;
}

static void			engine_load_env1(void)
{
	load_shaders("render",
		"/src/shaders/render.vert", "/src/shaders/render.frag");
	load_shaders("default",
		"/src/shaders/default.vert", "/src/shaders/default.frag");
	load_shaders("shadow",
		"/src/shaders/shadow.vert", "/src/shaders/shadow.frag");
	load_shaders("blur",
		"/src/shaders/blur.vert", "/src/shaders/blur.frag");
	engine_get()->brdf_lut = bmp_load("./res/brdfLUT.bmp");
	texture_set_parameters(engine_get()->brdf_lut, 2,
		(GLenum[2]){GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[2]){GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE});
	texture_load(engine_get()->brdf_lut);
}

void			engine_load_env(void)
{
	int				t;
	DIR				*dir;
	struct dirent	*e;
	char			*b;

	engine_load_env1();
	dir = opendir("./res/skybox");
	while ((e = readdir(dir)))
	{
		if (e->d_name[0] == '.')
			continue;
		t = cubemap_load("./res/skybox/", e->d_name);
		ezarray_push(&engine_get()->textures_env, &t);
		b = ft_strjoin(e->d_name, "/light");
		t = cubemap_load("./res/skybox/", b);
		free(b);
		ezarray_push(&engine_get()->textures_env, &t);
	}
	engine_get()->env = *((int*)ezarray_get_index(
		engine_get()->textures_env, 0));
	engine_get()->env_spec = *((int*)ezarray_get_index(
		engine_get()->textures_env, 1));
}
