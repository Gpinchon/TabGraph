/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 19:57:06 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 19:57:35 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>
#include <dirent.h>

static void		engine_load_env1(void)
{
	GLenum	v[0];
	char	*path;

	(void)v;
	load_shaders("render",
		"./res/shaders/render.vert", "./res/shaders/render.frag");
	load_shaders("default",
		"./res/shaders/default.vert", "./res/shaders/default.frag");
	load_shaders("shadow",
		"./res/shaders/shadow.vert", "./res/shaders/shadow.frag");
	load_shaders("blur",
		"./res/shaders/blur.vert", "./res/shaders/blur.frag");
	path = ft_strjoin(engine_get()->program_path, "./res/stupid.bmp");
	bmp_load(path, "stupid");
	free(path);
	path = ft_strjoin(engine_get()->program_path, "./res/brdfLUT.bmp");
	engine_get()->brdf_lut = bmp_load(path, "BrdfLUT");
	free(path);
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
