/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/14 18:15:57 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <dirent.h>

t_engine *g_engine = NULL;

/*
** engine is a singleton
*/
inline t_engine	*engine_get()
{
	if (!g_engine)
		g_engine = ft_memalloc(sizeof(t_engine));
	return (g_engine);
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
	engine->swap_interval = 0;
	engine->program_path = convert_backslash(getcwd(NULL, 4096));
}

void	engine_destroy()
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

void	load_side(int texture_index, char *path, GLenum side)
{
	texture_assign(load_bmp(path), texture_index, side);
	free(path);
}

int	load_cubemap(char *path, char *name)
{
	int	t;
	t = texture_create(new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
	load_side(t, ft_strjoinfreebool(path, ft_strjoin(name, "/X+.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	load_side(t, ft_strjoinfreebool(path, ft_strjoin(name, "/X-.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	load_side(t, ft_strjoinfreebool(path, ft_strjoin(name, "/Y-.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	load_side(t, ft_strjoinfreebool(path, ft_strjoin(name, "/Y+.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	load_side(t, ft_strjoinfreebool(path, ft_strjoin(name, "/Z+.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	load_side(t, ft_strjoinfreebool(path, ft_strjoin(name, "/Z-.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	texture_generate_mipmap(t);
	return (t);
}

void engine_load_env()
{
	int		t;
	DIR		*dir;
	struct	dirent *e;

	dir = opendir("./res/skybox");
	while ((e = readdir(dir)))
	{
		if (e->d_name[0] == '.')
			continue;
		t = load_cubemap("./res/skybox/", e->d_name);
		ezarray_push(&engine_get()->textures_env, &t);
		t = texture_create(new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
		load_side(t, ft_strjoinfreebool("./res/skybox/", ft_strjoin(e->d_name, "/X+_spec.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		load_side(t, ft_strjoinfreebool("./res/skybox/", ft_strjoin(e->d_name, "/X-_spec.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		load_side(t, ft_strjoinfreebool("./res/skybox/", ft_strjoin(e->d_name, "/Y-_spec.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		load_side(t, ft_strjoinfreebool("./res/skybox/", ft_strjoin(e->d_name, "/Y+_spec.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		load_side(t, ft_strjoinfreebool("./res/skybox/", ft_strjoin(e->d_name, "/Z+_spec.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		load_side(t, ft_strjoinfreebool("./res/skybox/", ft_strjoin(e->d_name, "/Z-_spec.bmp"), 0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		texture_generate_mipmap(t);
		ezarray_push(&engine_get()->textures_env, &t);
	}
	engine_get()->env = *((int*)ezarray_get_index(engine_get()->textures_env, 0));
	engine_get()->env_spec = *((int*)ezarray_get_index(engine_get()->textures_env, 1));
}

void	engine_set_key_callback(SDL_Scancode keycode, kcallback callback)
{
	engine_get()->kcallbacks[keycode] = callback;
}
