/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/22 22:08:28 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

t_material	new_material(char *name)
{
	t_material	mtl;

	ft_memset(&mtl, 0, sizeof(t_material));
	ft_memset(&mtl.data, -1, sizeof(t_mtl));
	mtl.data.stupidity = 0;
	mtl.data.alpha = 1;
	mtl.data.parallax = 0.01;
	mtl.data.albedo = new_vec3(0.1, 0.1, 0.1);
	mtl.data.emitting = new_vec3(0, 0, 0);
	mtl.data.metallic = 0;
	mtl.data.roughness = 0.5;
	mtl.data.refraction = 1.5;
	mtl.name = new_ezstring(name);
	mtl.id = hash((unsigned char *)mtl.name.tostring);
	return (mtl);
}

void	material_assign_shader(int material_index, int shader_index)
{
	t_shader	*shader;
	t_material	*material;

	shader = ezarray_get_index(engine_get()->shaders, shader_index);
	material = ezarray_get_index(engine_get()->materials, material_index);
	if (!material || !shader)
		return;
	material->shader_index = shader_index;
	material->in_campos = shader_get_uniform_index(shader_index, "in_CamPos");
	material->in_transform = shader_get_uniform_index(shader_index, "in_Transform");
	material->in_modelmatrix = shader_get_uniform_index(shader_index, "in_ModelMatrix");
	material->in_normalmatrix = shader_get_uniform_index(shader_index, "in_NormalMatrix");
	material->in_albedo = shader_get_uniform_index(shader_index, "in_Albedo");
	material->in_emitting = shader_get_uniform_index(shader_index, "in_Emitting");
	material->in_uvmax = shader_get_uniform_index(shader_index, "in_UVMax");
	material->in_uvmin = shader_get_uniform_index(shader_index, "in_UVMin");
	material->in_roughness = shader_get_uniform_index(shader_index, "in_Roughness");
	material->in_metallic = shader_get_uniform_index(shader_index, "in_Metallic");
	material->in_refraction = shader_get_uniform_index(shader_index, "in_Refraction");
	material->in_alpha = shader_get_uniform_index(shader_index, "in_Alpha");
	material->in_parallax = shader_get_uniform_index(shader_index, "in_Parallax");
	material->in_stupidity = shader_get_uniform_index(shader_index, "in_Stupidity");
	material->in_texture_stupid = shader_get_uniform_index(shader_index, "in_Texture_Stupid");
	material->in_texture_albedo = shader_get_uniform_index(shader_index, "in_Texture_Albedo");
	material->in_texture_roughness = shader_get_uniform_index(shader_index, "in_Texture_Roughness");
	material->in_texture_metallic = shader_get_uniform_index(shader_index, "in_Texture_Metallic");
	material->in_texture_emitting = shader_get_uniform_index(shader_index, "in_Texture_Emitting");
	material->in_texture_normal = shader_get_uniform_index(shader_index, "in_Texture_Normal");
	material->in_texture_height = shader_get_uniform_index(shader_index, "in_Texture_Height");
	material->in_use_texture_albedo = shader_get_uniform_index(shader_index, "in_Use_Texture_Albedo");
	material->in_use_texture_roughness = shader_get_uniform_index(shader_index, "in_Use_Texture_Roughness");
	material->in_use_texture_metallic = shader_get_uniform_index(shader_index, "in_Use_Texture_Metallic");
	material->in_use_texture_emitting = shader_get_uniform_index(shader_index, "in_Use_Texture_Emitting");
	material->in_use_texture_normal = shader_get_uniform_index(shader_index, "in_Use_Texture_Normal");
	material->in_use_texture_height = shader_get_uniform_index(shader_index, "in_Use_Texture_Height");
	material->in_texture_env = shader_get_uniform_index(shader_index, "in_Texture_Env");
	material->in_texture_env_spec = shader_get_uniform_index(shader_index, "in_Texture_Env_Spec");
	material->in_texture_shadow = shader_get_uniform_index(shader_index, "in_Texture_Shadow");
	material->in_shadowtransform = shader_get_uniform_index(shader_index, "in_ShadowTransform");
}

int			material_get_index_by_name(char *name)
{
	int			i;
	ULL			h;
	t_material	*m;

	i = 0;
	h = hash((unsigned char*)name);
	while ((m = ezarray_get_index(engine_get()->materials, i)))
	{
		if (h == m->id)
			return (i);
		i++;
	}
	return (-1);
}

int			material_get_index_by_id(ULL h)
{
	int			i;
	t_material	*m;

	i = 0;
	while ((m = ezarray_get_index(engine_get()->materials, i)))
	{
		if (h == m->id)
			return (i);
		i++;
	}
	return (0);
}

void	material_set_textures(int material_index)
{
	t_material	*material;
	int			use_texture;
	int			*shader_textures;
	int			*textures;
	int			i;

	material = ezarray_get_index(engine_get()->materials, material_index);
	shader_textures = &material->in_texture_albedo;
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 6)
	{
		shader_set_texture(material->shader_index, shader_textures[i * 2 + 0], textures[i], GL_TEXTURE0 + i);
		use_texture = textures[i] == -1 ? 0 : 1;
		shader_set_uniform(material->shader_index, shader_textures[i * 2 + 1], &use_texture);
		i++;
	}
	shader_set_texture(material->shader_index, material->in_texture_env, engine_get()->env, GL_TEXTURE0 + i);
	shader_set_texture(material->shader_index, material->in_texture_env_spec, engine_get()->env_spec, GL_TEXTURE0 + i + 1);
	shader_set_texture(material->shader_index, material->in_texture_stupid, material->data.texture_stupid, GL_TEXTURE0 + i + 2);
}

void	material_set_uniforms(int material_index)
{
	t_material	*material;

	material = ezarray_get_index(engine_get()->materials, material_index);
	if (!material)
		return;
	material_set_textures(material_index);
	shader_set_uniform(material->shader_index, material->in_albedo, &material->data.albedo);
	shader_set_uniform(material->shader_index, material->in_emitting, &material->data.emitting);
	shader_set_uniform(material->shader_index, material->in_alpha, &material->data.alpha);
	shader_set_uniform(material->shader_index, material->in_roughness, &material->data.roughness);
	shader_set_uniform(material->shader_index, material->in_metallic, &material->data.metallic);
	shader_set_uniform(material->shader_index, material->in_refraction, &material->data.refraction);
	shader_set_uniform(material->shader_index, material->in_alpha, &material->data.alpha);
	shader_set_uniform(material->shader_index, material->in_parallax, &material->data.parallax);
	shader_set_uniform(material->shader_index, material->in_stupidity, &material->data.stupidity);
}

void	material_load_textures(int material_index)
{
	t_material	*material;
	int			*textures;
	int			i;

	material = ezarray_get_index(engine_get()->materials, material_index);
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 7)
	{
		texture_load(textures[i]);
		i++;
	}
}
