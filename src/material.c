/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/25 22:40:52 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int		material_create(char *name)
{
	t_material	mtl;

	ft_memset(&mtl, 0, sizeof(t_material));
	ft_memset(&mtl.data, -1, sizeof(t_mtl));
	mtl.data.texture_stupid = texture_get_by_name("./res/stupid.bmp");
	mtl.data.stupidity = 0;
	mtl.data.alpha = 1;
	mtl.data.parallax = 0.01;
	mtl.data.albedo = new_vec3(0.5, 0.5, 0.5);
	mtl.data.emitting = new_vec3(0, 0, 0);
	mtl.data.metallic = 0;
	mtl.data.roughness = 0.5;
	mtl.data.specular = new_vec3(0.04, 0.04, 0.04);
	mtl.name = new_ezstring(name);
	mtl.id = hash((unsigned char *)mtl.name.tostring);
	ezarray_push(&engine_get()->materials, &mtl);
	material_assign_shader(engine_get()->materials.length - 1, shader_get_by_name("default"));
	return (engine_get()->materials.length - 1);
}

/*static inline void	material_assign_shader1(t_material *m, int si)
{
	m->in_texture_albedo = shader_get_uniform_index(si, "in_Texture_Albedo");
	m->in_use_texture_albedo = shader_get_uniform_index(si, "in_Texture_Specular");
	m->in_texture_specular = shader_get_uniform_index(si, "in_Texture_Roughness");
	m->in_use_texture_specular = shader_get_uniform_index(si, "in_Texture_Metallic");
	m->in_texture_roughness = shader_get_uniform_index(si, "in_Texture_Emitting");
	m->in_use_texture_roughness = shader_get_uniform_index(si, "in_Texture_Normal");
	m->in_texture_metallic = shader_get_uniform_index(si, "in_Texture_Height");
	m->in_use_texture_metallic = shader_get_uniform_index(si, "in_Texture_AO");
	m->in_texture_emitting = shader_get_uniform_index(si, "in_Use_Texture_Albedo");
	m->in_use_texture_emitting = shader_get_uniform_index(si, "in_Use_Texture_Specular");
	m->in_texture_normal = shader_get_uniform_index(si, "in_Use_Texture_Roughness");
	m->in_use_texture_normal = shader_get_uniform_index(si, "in_Use_Texture_Metallic");
	m->in_texture_height = shader_get_uniform_index(si, "in_Use_Texture_Emitting");
	m->in_use_texture_height = shader_get_uniform_index(si, "in_Use_Texture_Normal");
	m->in_texture_ao = shader_get_uniform_index(si, "in_Use_Texture_Height");
	m->in_use_texture_ao = shader_get_uniform_index(si, "in_Use_Texture_AO");
	m->in_texture_env = shader_get_uniform_index(si, "in_Texture_Env");
	m->in_texture_env_spec = shader_get_uniform_index(si, "in_Texture_Env_Spec");
	m->in_texture_shadow = shader_get_uniform_index(si, "in_Texture_Shadow");
	m->in_shadowtransform = shader_get_uniform_index(si, "in_ShadowTransform");
}*/

void				material_assign_shader(int material_index, int si)
{
	t_material	*m;

	m = ezarray_get_index(engine_get()->materials, material_index);
	if (!m)
		return ;
	m->shader_index = si;
	/*m->in_campos = shader_get_uniform_index(si, "in_Campos");
	m->in_transform = shader_get_uniform_index(si, "in_Transform");
	m->in_modelmatrix = shader_get_uniform_index(si, "in_ModelMatrix");
	m->in_normalmatrix = shader_get_uniform_index(si, "in_NormalMatrix");
	m->shader_in[16] = shader_get_uniform_index(si, "in_Albedo");
	m->shader_in[18] = shader_get_uniform_index(si, "in_Emitting");
	m->in_uvmin = shader_get_uniform_index(si, "in_UVMax");
	m->in_uvmax = shader_get_uniform_index(si, "in_UVMin");
	m->shader_in[18] = shader_get_uniform_index(si, "in_Roughness");
	m->shader_in[19] = shader_get_uniform_index(si, "in_Metallic");
	m->shader_in[17] = shader_get_uniform_index(si, "in_Specular");
	m->shader_in[20] = shader_get_uniform_index(si, "in_Alpha");
	m->shader_in[21] = shader_get_uniform_index(si, "in_Parallax");
	m->in_stupidity = shader_get_uniform_index(si, "in_Stupidity");
	m->in_texture_stupid = shader_get_uniform_index(si, "in_Texture_Stupid");
	m->in_texture_brdf = shader_get_uniform_index(si, "in_Texture_BRDF");
	material_assign_shader1(m, si);*/
	m->shader_in[0] = shader_get_uniform_index(si, "in_Texture_Albedo");
	m->shader_in[1] = shader_get_uniform_index(si, "in_Use_Texture_Albedo");
	m->shader_in[2] = shader_get_uniform_index(si, "in_Texture_Specular");
	m->shader_in[3] = shader_get_uniform_index(si, "in_Use_Texture_Specular");
	m->shader_in[4] = shader_get_uniform_index(si, "in_Texture_Roughness");
	m->shader_in[5] = shader_get_uniform_index(si, "in_Use_Texture_Roughness");
	m->shader_in[6] = shader_get_uniform_index(si, "in_Texture_Metallic");
	m->shader_in[7] = shader_get_uniform_index(si, "in_Use_Texture_Metallic");
	m->shader_in[8] = shader_get_uniform_index(si, "in_Texture_Emitting");
	m->shader_in[9] = shader_get_uniform_index(si, "in_Use_Texture_Emitting");
	m->shader_in[10] = shader_get_uniform_index(si, "in_Texture_Normal");
	m->shader_in[11] = shader_get_uniform_index(si, "in_Use_Texture_Normal");
	m->shader_in[12] = shader_get_uniform_index(si, "in_Texture_Height");
	m->shader_in[13] = shader_get_uniform_index(si, "in_Use_Texture_Height");
	m->shader_in[14] = shader_get_uniform_index(si, "in_Texture_AO");
	m->shader_in[15] = shader_get_uniform_index(si, "in_Use_Texture_AO");
	m->shader_in[16] = shader_get_uniform_index(si, "in_Albedo");
	m->shader_in[17] = shader_get_uniform_index(si, "in_Specular");
	m->shader_in[18] = shader_get_uniform_index(si, "in_Emitting");
	m->shader_in[18] = shader_get_uniform_index(si, "in_Roughness");
	m->shader_in[19] = shader_get_uniform_index(si, "in_Metallic");
	m->shader_in[20] = shader_get_uniform_index(si, "in_Alpha");
	m->shader_in[21] = shader_get_uniform_index(si, "in_Parallax");
	m->in_uvmax = shader_get_uniform_index(si, "in_UVMax");
	m->in_uvmin = shader_get_uniform_index(si, "in_UVMin");
	m->in_campos = shader_get_uniform_index(si, "in_CamPos");
	m->in_transform = shader_get_uniform_index(si, "in_Transform");
	m->in_modelmatrix = shader_get_uniform_index(si, "in_ModelMatrix");
	m->in_normalmatrix = shader_get_uniform_index(si, "in_NormalMatrix");

	m->in_stupidity = shader_get_uniform_index(si, "in_Stupidity");
	m->in_texture_stupid = shader_get_uniform_index(si, "in_Texture_Stupid");
	m->in_texture_brdf = shader_get_uniform_index(si, "in_Texture_BRDF");
	m->in_texture_env = shader_get_uniform_index(si, "in_Texture_Env");
	m->in_texture_env_spec = shader_get_uniform_index(si, "in_Texture_Env_Spec");
	m->in_texture_shadow = shader_get_uniform_index(si, "in_Texture_Shadow");
	m->in_shadowtransform = shader_get_uniform_index(si, "in_ShadowTransform");
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

void	material_bind_textures(int material_index)
{
	t_material	*material;
	int			use_texture;
	short		*shader_textures;
	short		*textures;
	short		i;

	material = ezarray_get_index(engine_get()->materials, material_index);
	shader_textures = &material->shader_in[0];
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 8)
	{
		shader_bind_texture(material->shader_index, shader_textures[i * 2 + 0], textures[i], GL_TEXTURE1 + i);
		use_texture = textures[i] == -1 ? 0 : 1;
		shader_set_int(material->shader_index, shader_textures[i * 2 + 1], use_texture);
		i++;
	}
	shader_bind_texture(material->shader_index, material->in_texture_env, engine_get()->env, GL_TEXTURE11);
	shader_bind_texture(material->shader_index, material->in_texture_env_spec, engine_get()->env_spec, GL_TEXTURE12);
	shader_bind_texture(material->shader_index, material->in_texture_brdf, engine_get()->brdf_lut, GL_TEXTURE13);
	shader_bind_texture(material->shader_index, material->in_texture_stupid, material->data.texture_stupid, GL_TEXTURE14);
}

void	material_set_uniforms(int material_index)
{
	t_material	*material;

	material = ezarray_get_index(engine_get()->materials, material_index);
	if (!material)
		return ;
	material_bind_textures(material_index);
	shader_set_vec3(material->shader_index, material->shader_in[16], material->data.albedo);
	shader_set_vec3(material->shader_index, material->shader_in[17], material->data.specular);
	shader_set_vec3(material->shader_index, material->shader_in[18], material->data.emitting);
	shader_set_float(material->shader_index, material->shader_in[20], material->data.alpha);
	shader_set_float(material->shader_index, material->shader_in[18], material->data.roughness);
	shader_set_float(material->shader_index, material->shader_in[19], material->data.metallic);
	shader_set_float(material->shader_index, material->shader_in[21], material->data.parallax);
	shader_set_float(material->shader_index, material->in_stupidity, material->data.stupidity);
}

void	material_load_textures(int material_index)
{
	t_material	*material;
	short		*textures;
	short		i;

	material = ezarray_get_index(engine_get()->materials, material_index);
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 9)
	{
		texture_load(textures[i]);
		i++;
	}
}
