/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/09 15:59:06 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

/*
** engine is a singleton
*/
t_engine	*engine_init()
{
	static t_engine	*engine = NULL;

	if (engine || !(engine = ft_memalloc(sizeof(t_engine))))
		return (engine);
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, MSAA);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	engine->cameras = new_ezarray(other, 0, sizeof(t_camera));
	engine->shaders = new_ezarray(other, 0, sizeof(t_shader));
	engine->textures = new_ezarray(other, 0, sizeof(t_texture));
	engine->materials = new_ezarray(other, 0, sizeof(t_material));
	engine->meshes = new_ezarray(other, 0, sizeof(t_mesh));
	engine->transforms = new_ezarray(other, 0, sizeof(t_transform));
	engine->lights = new_ezarray(other, 0, sizeof(t_light));
	engine->loop = 1;
	engine->swap_interval = 1;
	g_program_path = convert_backslash(getcwd(NULL, 2048));
	return (engine);
}

void engine_load_env(t_engine *e)
{
	e->env = texture_create(e, new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/X+.bmp"), e->env, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/X-.bmp"), e->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/Y-.bmp"), e->env, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/Y+.bmp"), e->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/Z+.bmp"), e->env, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/Z-.bmp"), e->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	texture_generate_mipmap(e, e->env);
	e->env_spec = texture_create(e, new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/X+_spec.bmp"), e->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/X-_spec.bmp"), e->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/Y-_spec.bmp"), e->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/Y+_spec.bmp"), e->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/Z+_spec.bmp"), e->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	texture_assign(e, load_bmp(e, "./res/skybox/hell/Z-_spec.bmp"), e->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	texture_generate_mipmap(e, e->env_spec);
}

void	engine_set_key_callback(t_engine *engine, SDL_Scancode keycode, kcallback callback)
{
	engine->kcallbacks[keycode] = callback;
}
