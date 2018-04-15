/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 19:57:33 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

t_engine	*g_engine = NULL;

/*
** engine is a singleton
*/

inline t_engine	*engine_get(void)
{
	if (!g_engine)
		g_engine = calloc(1, sizeof(t_engine));
	return (g_engine);
}

static void	set_program_path(char *argv0)
{
	int			len;

	len = strlen(argv0);
	len--;
	while (len >= 0 && argv0[len] != '/' && argv0[len] != '\\')
		len--;
	argv0[len + 1] = 0;
	engine_get()->program_path = convert_backslash(argv0);
}

static void	setup_sdl()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, MSAA);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE);
}

void		engine_init(char *program_path)
{
	t_engine	*engine;

	engine = engine_get();
	if (!engine)
		return ;
	setup_sdl();
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
	set_program_path(program_path);
}

void			engine_destroy(void)
{
	t_engine *engine;

	cleanup();
	engine = engine_get();
	free(engine);
	g_engine = NULL;
}
