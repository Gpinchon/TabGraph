/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/09 19:59:57 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <dirent.h>

t_engine	*g_engine = NULL;

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
	engine->internal_quality = 0.5;
	engine->exec_path = convert_backslash(getcwd(NULL, 4096));
	engine->exec_path = ft_strjoinfreebool(engine->exec_path, "/", 1, 0);
}

void			engine_destroy(void)
{
	t_engine *engine;

	cleanup();
	engine = engine_get();
	free(engine);
	g_engine = NULL;
}

static void		engine_load_env1(void)
{
	GLenum	v[0];
	char	*path;

	(void)v;
	load_shaders("render",
		"/res/shaders/render.vert", "/res/shaders/render.frag");
	load_shaders("default",
		"/res/shaders/default.vert", "/res/shaders/default.frag");
	load_shaders("shadow",
		"/res/shaders/shadow.vert", "/res/shaders/shadow.frag");
	load_shaders("blur",
		"/res/shaders/blur.vert", "/res/shaders/blur.frag");
	path = ft_strjoin(engine_get()->program_path, "./res/stupid.bmp");
	bmp_load(path, "stupid");
	free(path);
	path = ft_strjoin(engine_get()->program_path, "./res/brdfLUT.bmp");
	engine_get()->brdf_lut = bmp_load(path, "BrdfLUT");
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
	char			*folder;

	engine_load_env1();
	folder = ft_strjoin(engine_get()->program_path, "res/skybox/");
	dir = opendir(folder);
	while ((e = readdir(dir)))
	{
		if (e->d_name[0] == '.')
			continue;
		t = cubemap_load(folder, e->d_name);
		ezarray_push(&engine_get()->textures_env, &t);
		b = ft_strjoin(e->d_name, "/light");
		t = cubemap_load(folder, b);
		free(b);
		ezarray_push(&engine_get()->textures_env, &t);
	}
	free(folder);
	engine_get()->env = *((int*)ezarray_get_index(
		engine_get()->textures_env, 0));
	engine_get()->env_spec = *((int*)ezarray_get_index(
		engine_get()->textures_env, 1));
}
