/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/01 01:14:58 by gpinchon         ###   ########.fr       */
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
	engine->swap_interval = 1;
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

int		vbuffer_load(GLuint attrib, int size, ARRAY array, GLuint *bufferid)
{
	GLuint	lbufferid = *bufferid;
	if (!size || !array.length)
		return (-1);
	if (glIsBuffer(lbufferid))
		glDeleteBuffers(1, &lbufferid);
	glGenBuffers(1, &lbufferid);
	glBindBuffer(GL_ARRAY_BUFFER, lbufferid);
	glBufferData(GL_ARRAY_BUFFER, array.total_size, array.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(attrib);
	glVertexAttribPointer(attrib, size, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*bufferid = lbufferid;
	return (0);
}

int		vgroup_load(t_vgroup *vg)
{
	t_vgroup	lvg;
	int			ret;

	lvg = *vg;
	ret = 0;
	if (glIsVertexArray(lvg.v_arrayid))
		glDeleteVertexArrays(1, &lvg.v_arrayid);
	glGenVertexArrays(1, &lvg.v_arrayid);
	glBindVertexArray(lvg.v_arrayid);
	ret = !ret | !vbuffer_load(0, 3, lvg.v, &lvg.v_bufferid);
	ret = !ret | !vbuffer_load(1, 3, lvg.vn, &lvg.vn_bufferid);
	ret = !ret | !vbuffer_load(2, 2, lvg.vt, &lvg.vt_bufferid);
	//ret = !ret | !vbuffer_load(3, 3, lvg.tan, &lvg.tan_bufferid);
	//ret = !ret | !vbuffer_load(4, 3, lvg.bitan, &lvg.bitan_bufferid);
	printf("Total Size %i, Buffer ID %i\n", lvg.v.total_size, lvg.v_bufferid);
	printf("Total Size %i, Buffer ID %i\n", lvg.vn.total_size, lvg.vn_bufferid);
	printf("Total Size %i, Buffer ID %i\n", lvg.vt.total_size, lvg.vt_bufferid);
	//printf("Total Size %i, Buffer ID %i\n", lvg.tan.total_size, lvg.tan_bufferid);
	//printf("Total Size %i, Buffer ID %i\n", lvg.bitan.total_size, lvg.bitan_bufferid);
	glBindVertexArray(0);
	*vg = lvg;
	return (ret);
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

void	set_shader_uniform(t_engine *engine, int shader_index, int uniform_index, void *value)
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

int		get_uniform_index(t_engine *engine, int shader_index, char *name)
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

GLuint			load_texture(t_texture *texture, GLenum target)
{
	GLenum format = GL_BGR;
	GLenum internal_format = GL_RGB;

	if (texture->loaded)
		return (texture->id_ogl);
	if (!texture)
		return (0);
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
	texture->target = target;
	glGenTextures(1, &texture->id_ogl);
	glBindTexture(target, texture->id_ogl);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	if (texture->bpp < 32)
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexImage2D(target, 0, internal_format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
	glGenerateMipmap(target);
	glBindTexture(target, 0);
	texture->loaded = 1;
	return (texture->id_ogl);
}

GLuint			assign_texture(t_texture *texture, GLuint id, GLenum target)
{
	GLenum format = GL_BGR;
	GLenum internal_format = GL_RGB;

	if (!texture)
		return (0);
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
	glBindTexture(target, id);
	glTexImage2D(target, 0, internal_format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
	glGenerateMipmap(target);
	glBindTexture(target, 0);
	return (id);
}

void	set_shader_texture(t_engine *engine, int shader_index, int uniform_index, t_texture *texture, GLenum texture_unit)
{
	if (!texture)
		return;
	glActiveTexture(texture_unit);
	glBindTexture(texture->target, texture->id_ogl);
	texture_unit -= GL_TEXTURE0;
	set_shader_uniform(engine, shader_index, uniform_index, &texture_unit);
}

GLuint	generate_texture(t_texture *texture)
{
	glGenTextures(1, &texture->id_ogl);
	glBindTexture(texture->target, texture->id_ogl);
	glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (texture->bpp < 32)
		glTexParameteri(texture->target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glBindTexture(texture->target, 0);
	return (texture->id_ogl);
}

void	assign_shader_to_vgroup(t_engine *engine, int mesh_index, int vgroup_index, int shader_index)
{
	t_mesh		*mesh;
	t_vgroup	*vgroup;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	vgroup = ezarray_get_index(mesh->vgroups, vgroup_index);
	if (!vgroup)
		return;
	vgroup->shader_index = shader_index;
	vgroup->in_campos = get_uniform_index(engine, shader_index, "in_CamPos");
	vgroup->in_transform = get_uniform_index(engine, shader_index, "in_Transform");
	vgroup->in_modelmatrix = get_uniform_index(engine, shader_index, "in_ModelMatrix");
	vgroup->in_normalmatrix = get_uniform_index(engine, shader_index, "in_NormalMatrix");
	vgroup->in_albedo = get_uniform_index(engine, shader_index, "in_Albedo");
	vgroup->in_emitting = get_uniform_index(engine, shader_index, "in_Emitting");
	vgroup->in_uvmax = get_uniform_index(engine, shader_index, "in_UVMax");
	vgroup->in_uvmin = get_uniform_index(engine, shader_index, "in_UVMin");
	vgroup->in_roughness = get_uniform_index(engine, shader_index, "in_Roughness");
	vgroup->in_metallic = get_uniform_index(engine, shader_index, "in_Metallic");
	vgroup->in_refraction = get_uniform_index(engine, shader_index, "in_Refraction");
	vgroup->in_alpha = get_uniform_index(engine, shader_index, "in_Alpha");
	vgroup->in_parallax = get_uniform_index(engine, shader_index, "in_Parallax");
	vgroup->in_texture_albedo = get_uniform_index(engine, shader_index, "in_Texture_Albedo");
	vgroup->in_texture_roughness = get_uniform_index(engine, shader_index, "in_Texture_Roughness");
	vgroup->in_texture_metallic = get_uniform_index(engine, shader_index, "in_Texture_Metallic");
	vgroup->in_texture_normal = get_uniform_index(engine, shader_index, "in_Texture_Normal");
	vgroup->in_texture_height = get_uniform_index(engine, shader_index, "in_Texture_Height");
	vgroup->in_use_texture_albedo = get_uniform_index(engine, shader_index, "in_Use_Texture_Albedo");
	vgroup->in_use_texture_roughness = get_uniform_index(engine, shader_index, "in_Use_Texture_Roughness");
	vgroup->in_use_texture_metallic = get_uniform_index(engine, shader_index, "in_Use_Texture_Metallic");
	vgroup->in_use_texture_normal = get_uniform_index(engine, shader_index, "in_Use_Texture_Normal");
	vgroup->in_use_texture_height = get_uniform_index(engine, shader_index, "in_Use_Texture_Height");
	vgroup->in_texture_env = get_uniform_index(engine, shader_index, "in_Texture_Env");
	vgroup->in_texture_env_spec = get_uniform_index(engine, shader_index, "in_Texture_Env_Spec");
}

void	assign_shader_to_mesh(t_engine *engine, int mesh_index, int shader_index)
{
	unsigned	vgroup_index;
	t_mesh		*mesh;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	if (!mesh)
		return;
	vgroup_index = 0;
	while(vgroup_index < mesh->vgroups.length)
	{
		assign_shader_to_vgroup(engine, mesh_index, vgroup_index, shader_index);
		vgroup_index++;
	}
}

void	load_material_textures(t_engine *engine, int material_index)
{
	t_texture	*texture;
	t_material	*material;
	int			*textures;
	int			i;

	material = ezarray_get_index(engine->materials, material_index);
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 5)
	{
		texture = ezarray_get_index(engine->textures, textures[i]);
		if (texture)
			load_texture(texture, texture->target);
		i++;
	}
}

void	set_shader_textures(t_engine *engine, t_vgroup *vgroup, t_material *material)
{
	t_texture	*texture;
	int			use_texture;
	int			*shader_textures;
	int			*textures;
	int			i;

	shader_textures = &vgroup->in_texture_albedo;
	textures = &material->data.texture_albedo;
	i = 0;
	while (i < 5)
	{
		texture = ezarray_get_index(engine->textures, textures[i]);
		set_shader_texture(engine, vgroup->shader_index, shader_textures[i * 2 + 0], texture, GL_TEXTURE0 + i);
		use_texture = texture ? 1 : 0;
		set_shader_uniform(engine, vgroup->shader_index, shader_textures[i * 2 + 1], &use_texture);
		i++;
	}
	set_shader_texture(engine, vgroup->shader_index, vgroup->in_texture_env, ezarray_get_index(engine->textures, engine->env), GL_TEXTURE0 + i);
	set_shader_texture(engine, vgroup->shader_index, vgroup->in_texture_env_spec, ezarray_get_index(engine->textures, engine->env_spec), GL_TEXTURE0 + i + 1);
}

void	vgroup_render(t_engine *engine, int camera_index, int mesh_index, int vgroup_index)
{
	t_mesh		*mesh;
	t_vgroup	*vgroup;
	t_shader	*shader;
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
	shader = ezarray_get_index(engine->shaders, vgroup->shader_index);
	if (!shader)
		return;
	load_material_textures(engine, vgroup->mtl_index);
	t_transform *t = ezarray_get_index(engine->transforms, mesh->transform_index);

	MAT4 transform;
	transform = mat4_combine(camera->projection, camera->view, t->transform);
	MAT4	normal_matrix = mat4_transpose(mat4_inverse(t->transform));

	glUseProgram(shader->program);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	set_shader_textures(engine, vgroup, material);

	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_campos, &camera->position);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_modelmatrix, &t->transform);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_normalmatrix, &normal_matrix);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_transform, &transform);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_uvmin, &vgroup->uvmin);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_uvmax, &vgroup->uvmax);

	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_albedo, &material->data.albedo);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_emitting, &material->data.emitting);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_alpha, &material->data.alpha);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_roughness, &material->data.roughness);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_metallic, &material->data.metallic);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_refraction, &material->data.refraction);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_alpha, &material->data.alpha);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_parallax, &material->data.parallax);
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

void load_env(t_engine *engine)
{
	int X0 = load_bmp(engine, "./res/skybox/museum/X+.bmp"), X1 = load_bmp(engine, "./res/skybox/museum/X-.bmp"),
	Y0 = load_bmp(engine, "./res/skybox/museum/Y-.bmp"), Y1 = load_bmp(engine, "./res/skybox/museum/Y+.bmp"),
	Z0 = load_bmp(engine, "./res/skybox/museum/Z+.bmp"), Z1 = load_bmp(engine, "./res/skybox/museum/Z-.bmp");
	int X0_spec = load_bmp(engine, "./res/skybox/museum/X+_spec.bmp"), X1_spec = load_bmp(engine, "./res/skybox/museum/X-_spec.bmp"),
	Y0_spec = load_bmp(engine, "./res/skybox/museum/Y-_spec.bmp"), Y1_spec = load_bmp(engine, "./res/skybox/museum/Y+_spec.bmp"),
	Z0_spec = load_bmp(engine, "./res/skybox/museum/Z+_spec.bmp"), Z1_spec = load_bmp(engine, "./res/skybox/museum/Z-_spec.bmp");


	t_texture	env;
	env.target = GL_TEXTURE_CUBE_MAP;
	generate_texture(&env);
	glBindTexture(env.target, env.id_ogl);
	assign_texture(ezarray_get_index(engine->textures, X0), env.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	assign_texture(ezarray_get_index(engine->textures, X1), env.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	assign_texture(ezarray_get_index(engine->textures, Y0), env.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	assign_texture(ezarray_get_index(engine->textures, Y1), env.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	assign_texture(ezarray_get_index(engine->textures, Z0), env.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	assign_texture(ezarray_get_index(engine->textures, Z1), env.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	glGenerateMipmap(env.target);
	glBindTexture(env.target, 0);
	ezarray_push(&engine->textures, &env);
	engine->env = engine->textures.length - 1;

	env.target = GL_TEXTURE_CUBE_MAP;
	generate_texture(&env);
	glBindTexture(env.target, env.id_ogl);
	assign_texture(ezarray_get_index(engine->textures, X0_spec), env.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	assign_texture(ezarray_get_index(engine->textures, X1_spec), env.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	assign_texture(ezarray_get_index(engine->textures, Y0_spec), env.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	assign_texture(ezarray_get_index(engine->textures, Y1_spec), env.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	assign_texture(ezarray_get_index(engine->textures, Z0_spec), env.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	assign_texture(ezarray_get_index(engine->textures, Z1_spec), env.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	glGenerateMipmap(env.target);
	glBindTexture(env.target, 0);
	ezarray_push(&engine->textures, &env);
	engine->env_spec = engine->textures.length - 1;
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
	//t_transform *transform = ezarray_get_index(engine->transforms, mesh_get_transform_index(engine, 0));
	//if (transform)
	//	transform->rotation.y = CYCLE(transform->rotation.y + 0.001 * (float)engine->delta_time, 0, 2 * M_PI);
	printf("\rFPS %i%c[2K", (int)(1000.f / (float)(engine->delta_time)), 27);
	return (0);
}

int	main_loop(t_engine *engine)
{
	unsigned ticks, last_ticks;

	SDL_GL_SetSwapInterval(engine->swap_interval);
	last_ticks = SDL_GetTicks();
	int frames = 0;
	SDL_SetEventFilter(event_callback, engine);

	t_texture color_texture;
	color_texture.target = GL_TEXTURE_2D;
	generate_texture(&color_texture);
	glBindTexture(GL_TEXTURE_2D, color_texture.id_ogl);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	t_texture depth_texture;
	depth_texture.target = GL_TEXTURE_2D;
	generate_texture(&depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture.id_ogl);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint frame_buffer = 0;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture.id_ogl, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture.id_ogl, 0);
	GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
	glDrawBuffers(2, DrawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	int shader_index = load_shaders(engine, "render", "/src/shaders/render.vert", "/src/shaders/render.frag");
	t_shader *render_shader = ezarray_get_index(engine->shaders, shader_index);
	while(engine->loop)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
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
		set_shader_texture(engine, shader_index,
			get_uniform_index(engine, shader_index, "in_Texture_Color"),
			&color_texture, GL_TEXTURE0);
		set_shader_texture(engine, shader_index,
			get_uniform_index(engine, shader_index, "in_Texture_Depth"),
			&depth_texture, GL_TEXTURE1);
		//set_shader_uniform(engine, shader_index, get_uniform_index(engine, shader_index, "in_Texture_Color"), );
		glBindVertexArray(render_quadid);
		glDrawArrays(GL_TRIANGLES, 0, 18);
		glBindVertexArray(0);
		glUseProgram(0);
		SDL_GL_SwapWindow(engine->window->sdl_window);
		last_ticks = ticks;
		frames++;
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
	assign_shader_to_mesh(e, obj, shader);
	//render_scene(e);
	main_loop(e);
	//mesh_render(e, obj);
	glDisableVertexAttribArray(0);
	//SDL_GL_SwapWindow(e->window->sdl_window);
	return (argc + argv[0][0]);
}