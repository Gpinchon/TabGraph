/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/03 00:11:25 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

char		*convert_backslash(char *str)
{
	unsigned int	i;

	i = 0;
	while(str[i])
	{
		if (str[i] == '\\')
			str[i] = '/';
		i++;
	}
	return (str);
}

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
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	engine->cameras = new_ezarray(other, 0, sizeof(t_camera));
	engine->shaders = new_ezarray(other, 0, sizeof(t_shader));
	engine->textures = new_ezarray(other, 0, sizeof(t_texture));
	engine->materials = new_ezarray(other, 1, sizeof(t_material));
	engine->meshes = new_ezarray(other, 0, sizeof(t_mesh));
	engine->transforms = new_ezarray(other, 0, sizeof(t_transform));
	engine->lights = new_ezarray(other, 0, sizeof(t_light));
	engine->loop = 1;
	engine->swap_interval = -1;
	g_program_path = convert_backslash(getcwd(NULL, 2048));
	return (engine);
}

/*
** quad is a singleton
*/

static float	*create_display_quad()
{
	static float	*quad = NULL;

	if (quad || !(quad = malloc(sizeof(float) * 12)))
		return (quad);
	quad[0] = -1.0f; quad[1] = -1.0f;
    quad[2] = 1.0f; quad[3] = -1.0f;
    quad[4] = -1.0f; quad[5] = 1.0f;
    quad[6] = -1.0f; quad[7] = 1.0f;
    quad[8] = 1.0f; quad[9] = -1.0f;
    quad[10] = 1.0f; quad[11] = 1.0f;
	return (quad);
}

/*
** window is a singleton
*/

t_window		*window_init(t_engine *engine, const char *name, int width, int height)
{
	static t_window	*window = NULL;

	if (window || !(window = ft_memalloc(sizeof(t_window))))
		return (window);
	window->sdl_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	window->gl_context = SDL_GL_CreateContext(window->sdl_window);
	glewExperimental = GL_TRUE;
	if (!window->sdl_window || glewInit() != GLEW_OK)
	{
		free(window);
		return (window = NULL);
	}
	window->display_quad = create_display_quad();
	return (engine->window = window);
}

int	transform_create(t_engine *e, VEC3 position, VEC3 rotation, VEC3 scale)
{
	t_transform	t;

	t = new_transform(position, rotation, scale, UP);
	transform_update(&t);
	ezarray_push(&e->transforms, &t);
	return (e->transforms.length - 1);
}

GLuint		vbuffer_load(GLuint attrib, int size, ARRAY array)
{
	GLuint	lbufferid;
	if (!size || !array.length)
		return (-1);
	glGenBuffers(1, &lbufferid);
	glBindBuffer(GL_ARRAY_BUFFER, lbufferid);
	glBufferData(GL_ARRAY_BUFFER, array.total_size, array.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(attrib);
	glVertexAttribPointer(attrib, size, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return (lbufferid);
}

int		vgroup_load(t_vgroup *vg)
{
	t_vgroup	lvg;

	lvg = *vg;
	if (glIsVertexArray(lvg.v_arrayid))
		glDeleteVertexArrays(1, &lvg.v_arrayid);
	glGenVertexArrays(1, &lvg.v_arrayid);
	glBindVertexArray(lvg.v_arrayid);
	if (glIsBuffer(lvg.v_bufferid))
		glDeleteBuffers(1, &lvg.v_bufferid);
	lvg.v_bufferid = vbuffer_load(0, 3, lvg.v);
	if (glIsBuffer(lvg.vn_bufferid))
		glDeleteBuffers(1, &lvg.v_bufferid);
	lvg.vn_bufferid = vbuffer_load(1, 3, lvg.vn);
	if (glIsBuffer(lvg.vt_bufferid))
		glDeleteBuffers(1, &lvg.v_bufferid);
	lvg.vt_bufferid = vbuffer_load(2, 2, lvg.vt);
	glBindVertexArray(0);
	*vg = lvg;
	return (0);
}

void		mesh_load(t_engine *engine, int mesh_index)
{
	GLuint	i;
	t_mesh	*mesh;

	i = 0;
	mesh = ezarray_get_index(engine->meshes, mesh_index);
	if (!mesh)
		return;
	while (i < mesh->vgroups.length)
	{
		vgroup_load(ezarray_get_index(mesh->vgroups, i));
		i++;
	}
}

void	shader_set_uniform(t_engine *engine, int shader_index, int uniform_index, void *value)
{
	t_shadervariable	*variable;
	t_shader			*shader;

	shader = ezarray_get_index(engine->shaders, shader_index);
	if (!shader)
		return ;
	variable = ezarray_get_index(shader->uniforms, uniform_index);
	if (!variable)
		return ;
	if (variable->type == GL_FLOAT_VEC3)
		glUniform3fv(variable->loc, 1, ((float*)value));
	else if (variable->type == GL_FLOAT_VEC2)
		glUniform2fv(variable->loc, 1, ((float*)value));
	else if (variable->type == GL_FLOAT_MAT4)
		glUniformMatrix4fv(variable->loc, 1, GL_FALSE, ((float*)value));
	else if ((variable->type == GL_INT || variable->type == GL_BOOL || variable->type == GL_SAMPLER_2D || variable->type == GL_SAMPLER_CUBE))
		glUniform1i(variable->loc, *((int*)value));
	else if (variable->type == GL_UNSIGNED_INT)
		glUniform1ui(variable->loc, *((int*)value));
	else if (variable->type == GL_FLOAT)
		glUniform1f(variable->loc, *((float*)value));
}

int		shader_get_uniform_index(t_engine *engine, int shader_index, char *name)
{
	t_shader			*shader;
	unsigned			i;
	ULL					h;

	shader = ezarray_get_index(engine->shaders, shader_index);
	if (!shader)
		return (-1);
	i = 0;
	h = hash((unsigned char*)name);
	while (i < shader->uniforms.length)
	{
		if (((t_shadervariable*)ezarray_get_index(shader->uniforms, i))->id == h)
			return (i);
		i++;
	}
	return (-1);
}

void	texture_load(t_engine *engine, int texture_index)
{
	t_texture	*texture;
	GLenum		format;
	GLenum		internal_format;

	texture = ezarray_get_index(engine->textures, texture_index);
	if (!texture || texture->loaded)
		return ;
	format = GL_BGR;
	internal_format = GL_RGB;
	if (texture->bpp == 8)
	{
		format = GL_RED;
		internal_format = GL_RED;
	}
	else if (texture->bpp == 32)
	{
		format = GL_BGRA;
		internal_format =  GL_RGBA;
	}
	glGenTextures(1, &texture->id_ogl);
	glBindTexture(texture->target, texture->id_ogl);
	glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	if (texture->bpp < 32)
		glTexParameteri(texture->target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexImage2D(texture->target, 0, internal_format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
	glGenerateMipmap(texture->target);
	glBindTexture(texture->target, 0);
	texture->loaded = 1;
	return ;
}

void	texture_assign(t_engine *engine, int texture_index, int dest_texture_index, GLenum target)
{
	t_texture	*texture;
	t_texture	*dest_texture;
	GLenum format;
	GLenum internal_format;
	
	texture = ezarray_get_index(engine->textures, texture_index);
	dest_texture = ezarray_get_index(engine->textures, dest_texture_index);
	if (!texture || !dest_texture)
		return ;
	format = GL_BGR;
	internal_format = GL_RGB;
	if (texture->bpp == 8)
	{
		format = GL_RED;
		internal_format = GL_RED;
	}
	else if (texture->bpp == 32)
	{
		format = GL_BGRA;
		internal_format =  GL_RGBA;
	}
	glBindTexture(dest_texture->target, dest_texture->id_ogl);
	glBindTexture(target, dest_texture->id_ogl);
	glTexImage2D(target, 0, internal_format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
	glBindTexture(target, 0);
	glBindTexture(dest_texture->target, 0);
	return ;
}

void	shader_set_texture(t_engine *engine, int shader_index, int uniform_index, int texture_index, GLenum texture_unit)
{
	t_texture *texture;

	texture = ezarray_get_index(engine->textures, texture_index);
	if (!texture)
		return;
	glActiveTexture(texture_unit);
	glBindTexture(texture->target, texture->id_ogl);
	texture_unit -= GL_TEXTURE0;
	shader_set_uniform(engine, shader_index, uniform_index, &texture_unit);
}

GLuint	texture_get_ogl_id(t_engine *engine, int texture_index)
{
	t_texture	*texture;

	texture = ezarray_get_index(engine->textures, texture_index);
	if (!texture)
		return (0);
	return (texture->id_ogl);
}

void	texture_set_parameters(t_engine *engine, int texture_index, int parameter_nbr, GLenum *parameters, GLenum *values)
{
	t_texture *texture;

	texture = ezarray_get_index(engine->textures, texture_index);
	if (!texture)
		return;
	glBindTexture(texture->target, texture->id_ogl);
	while (parameter_nbr > 0)
	{
		glTexParameteri(texture->target, parameters[parameter_nbr - 1], values[parameter_nbr - 1]);
		parameter_nbr--;
	}
	glBindTexture(texture->target, 0);
}

int		texture_create(t_engine *engine, VEC2 size, GLenum target, GLenum internal_format, GLenum format)
{
	t_texture	texture;

	ft_memset(&texture, 0, sizeof(t_texture));
	texture.target = target;
	glGenTextures(1, &texture.id_ogl);
	glBindTexture(texture.target, texture.id_ogl);
	if (size.x > 0 && size.y > 0)
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size.x, size.y, 0, format, GL_FLOAT, NULL);
	if (texture.bpp < 32)
		glTexParameteri(texture.target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glBindTexture(texture.target, 0);
	ezarray_push(&engine->textures, &texture);
	return (engine->textures.length - 1);
}

void	material_assign_shader(t_engine *engine, int material_index, int shader_index)
{
	t_shader	*shader;
	t_material	*material;

	shader = ezarray_get_index(engine->shaders, shader_index);
	material = ezarray_get_index(engine->materials, material_index);
	if (!material || !shader)
		return;
	material->shader_index = shader_index;
	material->in_campos = shader_get_uniform_index(engine, shader_index, "in_CamPos");
	material->in_transform = shader_get_uniform_index(engine, shader_index, "in_Transform");
	material->in_modelmatrix = shader_get_uniform_index(engine, shader_index, "in_ModelMatrix");
	material->in_normalmatrix = shader_get_uniform_index(engine, shader_index, "in_NormalMatrix");
	material->in_albedo = shader_get_uniform_index(engine, shader_index, "in_Albedo");
	material->in_emitting = shader_get_uniform_index(engine, shader_index, "in_Emitting");
	material->in_uvmax = shader_get_uniform_index(engine, shader_index, "in_UVMax");
	material->in_uvmin = shader_get_uniform_index(engine, shader_index, "in_UVMin");
	material->in_roughness = shader_get_uniform_index(engine, shader_index, "in_Roughness");
	material->in_metallic = shader_get_uniform_index(engine, shader_index, "in_Metallic");
	material->in_refraction = shader_get_uniform_index(engine, shader_index, "in_Refraction");
	material->in_alpha = shader_get_uniform_index(engine, shader_index, "in_Alpha");
	material->in_parallax = shader_get_uniform_index(engine, shader_index, "in_Parallax");
	material->in_texture_albedo = shader_get_uniform_index(engine, shader_index, "in_Texture_Albedo");
	material->in_texture_roughness = shader_get_uniform_index(engine, shader_index, "in_Texture_Roughness");
	material->in_texture_metallic = shader_get_uniform_index(engine, shader_index, "in_Texture_Metallic");
	material->in_texture_normal = shader_get_uniform_index(engine, shader_index, "in_Texture_Normal");
	material->in_texture_height = shader_get_uniform_index(engine, shader_index, "in_Texture_Height");
	material->in_use_texture_albedo = shader_get_uniform_index(engine, shader_index, "in_Use_Texture_Albedo");
	material->in_use_texture_roughness = shader_get_uniform_index(engine, shader_index, "in_Use_Texture_Roughness");
	material->in_use_texture_metallic = shader_get_uniform_index(engine, shader_index, "in_Use_Texture_Metallic");
	material->in_use_texture_normal = shader_get_uniform_index(engine, shader_index, "in_Use_Texture_Normal");
	material->in_use_texture_height = shader_get_uniform_index(engine, shader_index, "in_Use_Texture_Height");
	material->in_texture_env = shader_get_uniform_index(engine, shader_index, "in_Texture_Env");
	material->in_texture_env_spec = shader_get_uniform_index(engine, shader_index, "in_Texture_Env_Spec");
}

void	mesh_assign_shader(t_engine *engine, int mesh_index, int shader_index)
{
	unsigned	vgroup_index;
	t_mesh		*mesh;
	t_vgroup	*vgroup;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	if (!mesh)
		return;
	vgroup_index = 0;
	while(vgroup_index < mesh->vgroups.length)
	{
		vgroup = ezarray_get_index(mesh->vgroups, vgroup_index);
		if (vgroup)
			material_assign_shader(engine, vgroup->mtl_index, shader_index);
		vgroup_index++;
	}
}

void	load_material_textures(t_engine *engine, int material_index)
{
	t_material	*material;
	int			*textures;
	int			i;

	material = ezarray_get_index(engine->materials, material_index);
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 5)
	{
		texture_load(engine, textures[i]);
		i++;
	}
}

void	material_set_textures(t_engine *engine, int material_index)
{
	t_material	*material;
	int			use_texture;
	int			*shader_textures;
	int			*textures;
	int			i;

	material = ezarray_get_index(engine->materials, material_index);
	shader_textures = &material->in_texture_albedo;
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 5)
	{
		shader_set_texture(engine, material->shader_index, shader_textures[i * 2 + 0], textures[i], GL_TEXTURE0 + i);
		use_texture = textures[i] == -1 ? 0 : 1;
		shader_set_uniform(engine, material->shader_index, shader_textures[i * 2 + 1], &use_texture);
		i++;
	}
	shader_set_texture(engine, material->shader_index, material->in_texture_env, engine->env, GL_TEXTURE0 + i);
	shader_set_texture(engine, material->shader_index, material->in_texture_env_spec, engine->env_spec, GL_TEXTURE0 + i + 1);
}

void	material_set_uniforms(t_engine *engine, int material_index)
{
	t_material	*material;

	material = ezarray_get_index(engine->materials, material_index);
	if (!material)
		return;
	material_set_textures(engine, material_index);
	shader_set_uniform(engine, material->shader_index, material->in_albedo, &material->data.albedo);
	shader_set_uniform(engine, material->shader_index, material->in_emitting, &material->data.emitting);
	shader_set_uniform(engine, material->shader_index, material->in_alpha, &material->data.alpha);
	shader_set_uniform(engine, material->shader_index, material->in_roughness, &material->data.roughness);
	shader_set_uniform(engine, material->shader_index, material->in_metallic, &material->data.metallic);
	shader_set_uniform(engine, material->shader_index, material->in_refraction, &material->data.refraction);
	shader_set_uniform(engine, material->shader_index, material->in_alpha, &material->data.alpha);
	shader_set_uniform(engine, material->shader_index, material->in_parallax, &material->data.parallax);
}

void	shader_use(t_engine *engine, int shader_index)
{
	t_shader	*shader;

	shader = ezarray_get_index(engine->shaders, shader_index);
	if (!shader)
		return;
	glUseProgram(shader->program);
}

void	vgroup_render(t_engine *engine, int camera_index, int mesh_index, int vgroup_index)
{
	t_mesh		*mesh;
	t_vgroup	*vgroup;
	t_material	*material;
	t_camera	*camera;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	vgroup = ezarray_get_index(mesh->vgroups, vgroup_index);
	camera = ezarray_get_index(engine->cameras, camera_index);
	if (!mesh || !vgroup || !camera)
		return;
	material = ezarray_get_index(engine->materials, vgroup->mtl_index);
	if (!material)
		return;
	load_material_textures(engine, vgroup->mtl_index);
	t_transform *t = ezarray_get_index(engine->transforms, mesh->transform_index);

	MAT4 transform;
	transform = mat4_combine(camera->projection, camera->view, t->transform);
	MAT4	normal_matrix = mat4_transpose(mat4_inverse(t->transform));
	shader_use(engine, material->shader_index);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	material_set_uniforms(engine, vgroup->mtl_index);
	shader_set_uniform(engine, material->shader_index, material->in_campos, &camera->position);
	shader_set_uniform(engine, material->shader_index, material->in_modelmatrix, &t->transform);
	shader_set_uniform(engine, material->shader_index, material->in_normalmatrix, &normal_matrix);
	shader_set_uniform(engine, material->shader_index, material->in_transform, &transform);
	shader_set_uniform(engine, material->shader_index, material->in_uvmin, &vgroup->uvmin);
	shader_set_uniform(engine, material->shader_index, material->in_uvmax, &vgroup->uvmax);
	glBindVertexArray(vgroup->v_arrayid);
	glDrawArrays(GL_TRIANGLES, 0, vgroup->v.length);
	glBindVertexArray(0);
	glUseProgram(0);
}

void	mesh_render(t_engine *engine, int camera_index, int mesh_index)
{
	t_mesh		*mesh;
	unsigned	vgroup_index;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	if (!mesh)
		return ;
	transform_update(ezarray_get_index(engine->transforms, mesh->transform_index));
	vgroup_index = 0;
	while (vgroup_index < mesh->vgroups.length)
	{
		vgroup_render(engine, camera_index, mesh_index, vgroup_index);
		vgroup_index++;
	}
}

void	camera_update(t_engine *engine, int camera_index)
{
	t_transform	*camera_target;
	VEC3		target;
	t_camera	*camera;
	
	camera = ezarray_get_index(engine->cameras, camera_index);
	if (!camera)
		return ;
	camera_target = ezarray_get_index(engine->transforms, camera->target_index);
	target = camera_target ? camera_target->position : new_vec3(0, 0, 0);
	camera->view = mat4_lookat(camera->position, target, UP);
	camera->projection = mat4_perspective(camera->fov, (float)WIDTH / (float)HEIGHT, 0.1, 1000);
	(void)camera_index;
}

int		camera_get_target_index(t_engine *engine, int camera_index)
{
	t_camera	*camera;

	camera = ezarray_get_index(engine->cameras, camera_index);
	if (!camera)
		return (-1);
	return (camera->target_index);
}

void	camera_set_target(t_engine *engine, int camera_index, int mesh_index)
{
	t_mesh		*mesh;
	t_camera	*camera;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	camera = ezarray_get_index(engine->cameras, camera_index);
	if (!mesh || !camera)
		return ;
	camera->target_index = mesh->transform_index;
}

void	camera_set_position(t_engine *engine, int camera_index, VEC3 position)
{
	t_camera	*camera;

	camera = ezarray_get_index(engine->cameras, camera_index);
	if (!camera)
		return;
	camera->position = position;
}

int		camera_create(t_engine *engine, float fov)
{
	t_camera	camera;

	ft_memset(&camera, 0, sizeof(t_camera));
	camera.fov = fov;
	camera.target_index = -1;
	ezarray_push(&engine->cameras, &camera);
	return (engine->cameras.length - 1);
}

void	scene_render(t_engine *engine, int camera_index)
{
	unsigned	mesh_index;

	mesh_index = 0;
	glClearColor(0.46f, 0.53f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera_update(engine, camera_index);
	while (mesh_index < engine->meshes.length)
	{
		mesh_render(engine, camera_index, mesh_index);
		mesh_index++;
	}
}

void	vgroup_center(t_engine *engine, int mesh_index, int vgroup_index)
{
	t_mesh		*mesh;
	t_vgroup	*vgroup;
	VEC3		*v;
	unsigned	v_index;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	vgroup = ezarray_get_index(mesh->vgroups, vgroup_index);
	if (!vgroup)
		return;
	v_index = 0;
	while (v_index < vgroup->v.length)
	{
		v = ezarray_get_index(vgroup->v, v_index);
		*v = vec3_sub(*v, mesh->bounding_box.center);
		v_index++;
	}
}

void	mesh_center(t_engine *engine, int mesh_index)
{
	t_mesh		*mesh;
	unsigned	vgroup_index;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	if (!mesh)
		return ;
	vgroup_index = 0;
	while (vgroup_index < mesh->vgroups.length)
	{
		vgroup_center(engine, mesh_index, vgroup_index);
		vgroup_index++;
	}
	mesh->bounding_box.center = new_vec3(0, 0, 0);
	mesh->bounding_box.min = vec3_sub(mesh->bounding_box.min, mesh->bounding_box.center);
	mesh->bounding_box.max = vec3_sub(mesh->bounding_box.max, mesh->bounding_box.center);
}

void	texture_generate_mipmap(t_engine *engine, int texture_index)
{
	t_texture *texture;

	texture = ezarray_get_index(engine->textures, texture_index);
	if (!texture)
		return;
	texture_set_parameters(engine, texture_index, 2,
		(GLenum[2]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER},
		(GLenum[2]){GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR});
	glBindTexture(texture->target, texture->id_ogl);
	glGenerateMipmap(texture->target);
	glBindTexture(texture->target, 0);
}

void load_env(t_engine *engine)
{
	int X0 = load_bmp(engine, "./res/skybox/cloudtop/X+.bmp"), X1 = load_bmp(engine, "./res/skybox/cloudtop/X-.bmp"),
	Y0 = load_bmp(engine, "./res/skybox/cloudtop/Y-.bmp"), Y1 = load_bmp(engine, "./res/skybox/cloudtop/Y+.bmp"),
	Z0 = load_bmp(engine, "./res/skybox/cloudtop/Z+.bmp"), Z1 = load_bmp(engine, "./res/skybox/cloudtop/Z-.bmp");
	int X0_spec = load_bmp(engine, "./res/skybox/cloudtop/X+_spec.bmp"), X1_spec = load_bmp(engine, "./res/skybox/cloudtop/X-_spec.bmp"),
	Y0_spec = load_bmp(engine, "./res/skybox/cloudtop/Y-_spec.bmp"), Y1_spec = load_bmp(engine, "./res/skybox/cloudtop/Y+_spec.bmp"),
	Z0_spec = load_bmp(engine, "./res/skybox/cloudtop/Z+_spec.bmp"), Z1_spec = load_bmp(engine, "./res/skybox/cloudtop/Z-_spec.bmp");

	engine->env = texture_create(engine, new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
	texture_assign(engine, X0, engine->env, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	texture_assign(engine, X1, engine->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	texture_assign(engine, Y0, engine->env, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	texture_assign(engine, Y1, engine->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	texture_assign(engine, Z0, engine->env, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	texture_assign(engine, Z1, engine->env, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	texture_generate_mipmap(engine, engine->env);

	engine->env_spec = texture_create(engine, new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
	texture_assign(engine, X0_spec, engine->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	texture_assign(engine, X1_spec, engine->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	texture_assign(engine, Y0_spec, engine->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	texture_assign(engine, Y1_spec, engine->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	texture_assign(engine, Z0_spec, engine->env_spec, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	texture_assign(engine, Z1_spec, engine->env_spec, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	texture_generate_mipmap(engine, engine->env_spec);
}

int	mesh_get_transform_index(t_engine *engine, int mesh_index)
{
	t_mesh *mesh;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	if (!mesh)
		return (-1);
	return (mesh->transform_index);
}

void	event_window(t_engine *engine, SDL_Event *event)
{
	if (event->window.event == SDL_WINDOWEVENT_CLOSE)
		engine->loop = 0;
}

void	camera_orbite(t_engine *engine, int camera_index, float phi, float theta, float radius)
{
	VEC3	target_position;
	VEC3	new_position;

	t_transform *target = ezarray_get_index(engine->transforms, camera_get_target_index(engine, camera_index));
	if (target)
		target_position = target->position;
	else
		target_position = new_vec3(0, 0, 0);
	new_position.x = target_position.x + radius * sin(phi) * cos(theta);
	new_position.z = target_position.y + radius * sin(phi) * sin(theta);
	new_position.y = target_position.z + radius * cos(phi);
	camera_set_position(engine, 0, new_position);
}

void	callback_camera(t_engine *engine, SDL_Event *event)
{
	static float phi = M_PI / 2.f;
	static float theta = M_PI / 2.f;
	static float radius = 5.f;
	if (event && event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_UP || event->key.keysym.sym == SDLK_DOWN)
			phi += (event->key.keysym.sym == SDLK_DOWN ? 0.1 : -0.1);
		else if (event->key.keysym.sym == SDLK_LEFT || event->key.keysym.sym == SDLK_RIGHT)
			theta += (event->key.keysym.sym == SDLK_LEFT ? 0.1 : -0.1);
		else if (event->key.keysym.sym == SDLK_KP_PLUS || event->key.keysym.sym == SDLK_KP_MINUS)
			radius += event->key.keysym.sym == SDLK_KP_PLUS ? -0.1 : 0.1;
		phi = CLAMP(phi, 0.01, M_PI - 0.01);
		theta = CYCLE(theta, 0, 2 * M_PI);
		radius = CLAMP(radius, 0.1f, 1000.f);
	}
	camera_orbite(engine, 0, phi, theta, radius);
}

void	engine_set_key_callback(t_engine *engine, SDL_Scancode keycode, kcallback callback)
{
	engine->kcallbacks[keycode] = callback;
}

void	event_keyboard(t_engine *engine, SDL_Event *event)
{
	if (engine->kcallbacks[event->key.keysym.scancode])
		engine->kcallbacks[event->key.keysym.scancode](engine, event);
}

int 	event_callback(void *e, SDL_Event *event)
{
	t_engine *engine;

	engine = e;
	if (event->type == SDL_QUIT)
		engine->loop = 0;
	else if (event->type == SDL_WINDOWEVENT)
		event_window(engine, event);
	else if (event->type == SDL_KEYUP
		|| event->type == SDL_KEYDOWN)
		event_keyboard(engine, event);
	return (0);
}

int		event_refresh(void *e)
{
	t_engine *engine;

	engine = e;
	printf("\rFPS %i%c[2K", (int)(1000.f / (float)(engine->delta_time)), 27);
	return (0);
}

t_framebuffer	build_render_buffer(t_engine *engine)
{
	t_framebuffer	framebuffer;

	framebuffer.texture_color = texture_create(engine, new_vec2(WIDTH, HEIGHT), GL_TEXTURE_2D, GL_RGB16F, GL_RGB);
	texture_set_parameters(engine, framebuffer.texture_color, 4, 
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP});
	framebuffer.texture_position = texture_create(engine, new_vec2(WIDTH, HEIGHT), GL_TEXTURE_2D, GL_RGB16F, GL_RGB);
	texture_set_parameters(engine, framebuffer.texture_position, 4,
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP});
	framebuffer.texture_normal = texture_create(engine, new_vec2(WIDTH, HEIGHT), GL_TEXTURE_2D, GL_RGB16F, GL_RGB);
	texture_set_parameters(engine, framebuffer.texture_normal, 4,
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP});
	framebuffer.texture_depth = texture_create(engine, new_vec2(WIDTH, HEIGHT), GL_TEXTURE_2D, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	texture_set_parameters(engine, framebuffer.texture_depth, 4,
		(GLenum[4]){GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T},
		(GLenum[4]){GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP});
	glGenFramebuffers(1, &framebuffer.id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_get_ogl_id(engine, framebuffer.texture_color), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture_get_ogl_id(engine, framebuffer.texture_normal), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texture_get_ogl_id(engine, framebuffer.texture_position), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_get_ogl_id(engine, framebuffer.texture_depth), 0);
	glDrawBuffers(3, (GLenum[3]){GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return (framebuffer);
}

int	main_loop(t_engine *engine)
{
	unsigned ticks, last_ticks;

	SDL_GL_SetSwapInterval(engine->swap_interval);
	last_ticks = SDL_GetTicks();
	SDL_SetEventFilter(event_callback, engine);


	GLuint bufferid, render_quadid;
	glGenVertexArrays(1, &render_quadid);
	glBindVertexArray(render_quadid);
	glGenBuffers(1, &bufferid);
	glBindBuffer(GL_ARRAY_BUFFER, bufferid);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), engine->window->display_quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	t_framebuffer renderbuffer = build_render_buffer(engine);
	int shader_index = load_shaders(engine, "render", "/src/shaders/render.vert", "/src/shaders/render.frag");
	t_shader *render_shader = ezarray_get_index(engine->shaders, shader_index);
	while(engine->loop)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderbuffer.id);
		ticks = SDL_GetTicks();
		engine->delta_time = ticks - last_ticks;
		SDL_PumpEvents();
		event_refresh(engine);
		glClearColor(engine->window->clear_color.x, engine->window->clear_color.y,
			engine->window->clear_color.z, engine->window->clear_color.w);
		glClear(engine->window->clear_mask);
		scene_render(engine, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(render_shader->program);
		glDisable(GL_DEPTH_TEST);
		shader_set_texture(engine, shader_index,
			shader_get_uniform_index(engine, shader_index, "in_Texture_Color"),
			renderbuffer.texture_color, GL_TEXTURE0);
		shader_set_texture(engine, shader_index,
			shader_get_uniform_index(engine, shader_index, "in_Texture_Normal"),
			renderbuffer.texture_normal, GL_TEXTURE1);
		shader_set_texture(engine, shader_index,
			shader_get_uniform_index(engine, shader_index, "in_Texture_Position"),
			renderbuffer.texture_normal, GL_TEXTURE2);
		shader_set_texture(engine, shader_index,
			shader_get_uniform_index(engine, shader_index, "in_Texture_Depth"),
			renderbuffer.texture_depth, GL_TEXTURE3);
		glBindVertexArray(render_quadid);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		glBindVertexArray(0);
		glUseProgram(0);
		SDL_GL_SwapWindow(engine->window->sdl_window);
		last_ticks = ticks;
	}
	return (0);
}

int main(int argc, char *argv[])
{
	t_engine	*e;

	if (argc < 2)
		return (0);
	e = engine_init();
	window_init(e, "Scope", WIDTH, HEIGHT);
	printf("%s\n", glGetString(GL_VERSION));
	load_env(e);
	int obj = load_obj(e, argv[1]);
	mesh_center(e, obj);
	int shader = load_shaders(e, "default", "/src/shaders/default.vert", "/src/shaders/default.frag");
	int camera = camera_create(e, 45);
	camera_set_target(e, camera, obj);
	//camera_set_position(e, camera, new_vec3(-3.5, 0.0, 3.5));
	camera_orbite(e, 0, M_PI / 2.f, M_PI / 2.f, 5.f);
	engine_set_key_callback(e, SDL_SCANCODE_UP, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_DOWN, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_LEFT, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_RIGHT, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_KP_PLUS, callback_camera);
	engine_set_key_callback(e, SDL_SCANCODE_KP_MINUS, callback_camera);	
	mesh_load(e, obj);
	mesh_assign_shader(e, obj, shader);
	//render_scene(e);
	main_loop(e);
	//mesh_render(e, obj);
	glDisableVertexAttribArray(0);
	//SDL_GL_SwapWindow(e->window->sdl_window);
	return (argc + argv[0][0]);
}