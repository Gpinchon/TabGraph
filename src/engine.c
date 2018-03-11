/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/10 12:25:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <dirent.h>

//t_engine *g_engine = NULL;

/*
** engine is a singleton
*/
inline t_engine	*engine_get()
{
	static t_engine	*engine = NULL;

	if (!engine)
		engine = ft_memalloc(sizeof(t_engine));
	return (engine);
}

void	engine_init()
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	engine->cameras = new_ezarray(other, 0, sizeof(t_camera));
	engine->shaders = new_ezarray(other, 0, sizeof(t_shader));
	engine->textures = new_ezarray(other, 0, sizeof(t_texture));
	engine->textures_env = new_ezarray(INT, 0);
	engine->materials = new_ezarray(other, 0, sizeof(t_material));
	engine->meshes = new_ezarray(other, 0, sizeof(t_mesh));
	engine->transforms = new_ezarray(other, 0, sizeof(t_transform));
	engine->lights = new_ezarray(other, 0, sizeof(t_light));
	engine->framebuffers = new_ezarray(other, 0, sizeof(t_framebuffer));
	engine->loop = 1;
	engine->swap_interval = 1;
	engine->program_path = convert_backslash(getcwd(NULL, 4096));
}

void engine_load_env()
{
	DIR *dir = opendir("./res/skybox");
	struct dirent *entry;
	char *buffer;
	while ((entry = readdir(dir)))
	{
		if (entry->d_name[0] == '.')
			continue;
		engine_get()->env = texture_create(new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
		buffer = ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/X+.bmp"), 0, 1);
		texture_assign(load_bmp(buffer), engine_get()->env, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		free(buffer);
		buffer = ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/X-.bmp"), 0, 1);
		texture_assign(load_bmp(buffer), engine_get()->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		free(buffer);
		buffer = ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/Y-.bmp"), 0, 1);
		texture_assign(load_bmp(buffer), engine_get()->env, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		free(buffer);
		buffer = ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/Y+.bmp"), 0, 1);
		texture_assign(load_bmp(buffer), engine_get()->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		free(buffer);
		buffer = ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/Z+.bmp"), 0, 1);
		texture_assign(load_bmp(buffer), engine_get()->env, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		free(buffer);
		buffer = ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/Z-.bmp"), 0, 1);
		texture_assign(load_bmp(buffer), engine_get()->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		free(buffer);
		texture_generate_mipmap(engine_get()->env);
		engine_get()->env_spec = texture_create(new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
		texture_assign(load_bmp(ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/X+_spec.bmp"), 0, 1)), engine_get()->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		texture_assign(load_bmp(ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/X-_spec.bmp"), 0, 1)), engine_get()->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		texture_assign(load_bmp(ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/Y-_spec.bmp"), 0, 1)), engine_get()->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		texture_assign(load_bmp(ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/Y+_spec.bmp"), 0, 1)), engine_get()->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		texture_assign(load_bmp(ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/Z+_spec.bmp"), 0, 1)), engine_get()->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		texture_assign(load_bmp(ft_strjoinfreebool("./res/skybox/", ft_strjoin(entry->d_name, "/Z-_spec.bmp"), 0, 1)), engine_get()->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		texture_generate_mipmap(engine_get()->env_spec);
		ezarray_push(&engine_get()->textures_env, &engine_get()->env);
		ezarray_push(&engine_get()->textures_env, &engine_get()->env_spec);
	}
	engine_get()->env = *((int*)ezarray_get_index(engine_get()->textures_env, 0));
	engine_get()->env_spec = *((int*)ezarray_get_index(engine_get()->textures_env, 1));
}

void	engine_set_key_callback(SDL_Scancode keycode, kcallback callback)
{
	engine_get()->kcallbacks[keycode] = callback;
}
