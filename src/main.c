/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/01/24 02:59:04 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int	main_loop(t_engine engine, t_window w)
{
	SDL_GL_SetSwapInterval(engine.swap_interval);
	while(engine.loop)
	{
		glClearColor(w.clear_color.x, w.clear_color.y,
			w.clear_color.z, w.clear_color.w);
		glClear(w.clear_mask);
		SDL_GL_SwapWindow(w.sdl_window);
	}
	return (0);
}

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
t_engine	*init_engine()
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
	engine->shaders = new_ezarray(other, 0, sizeof(t_shader));
	engine->textures = new_ezarray(other, 0, sizeof(t_texture));
	engine->materials = new_ezarray(other, 1, sizeof(t_material));
	engine->meshes = new_ezarray(other, 0, sizeof(t_mesh));
	engine->transforms = new_ezarray(other, 0, sizeof(t_transform));
	engine->lights = new_ezarray(other, 0, sizeof(t_light));
	engine->loop = 1;
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
	quad[0] = -1;
	quad[1] = -1;
	quad[2] = 1;
	quad[3] = -1;
	quad[4] = -1;
	quad[5] = 1;
	quad[6] = -1;
	quad[7] = 1;
	quad[8] = 1;
	quad[9] = -1;
	quad[10] = 1;
	quad[11] = 1;
	return (quad);
}

/*
** window is a singleton
*/

t_window		*init_window(t_engine *engine, const char *name, int width, int height)
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

int	create_transform(t_engine *e, VEC3 position, VEC3 rotation, VEC3 scale)
{
	t_transform	t;

	t = new_transform(position, rotation, scale, UP);
	transform_update(&t);
	ezarray_push(&e->transforms, &t);
	return (e->transforms.length - 1);
}

int		load_vbuffer(GLuint attrib, int size, ARRAY array, GLuint *bufferid)
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

int		load_vgroup(t_vgroup *vg)
{
	t_vgroup	lvg;
	int			ret;

	lvg = *vg;
	ret = 0;
	if (glIsVertexArray(lvg.v_arrayid))
		glDeleteVertexArrays(1, &lvg.v_arrayid);
	glGenVertexArrays(1, &lvg.v_arrayid);
	glBindVertexArray(lvg.v_arrayid);
	ret = !ret | !load_vbuffer(0, 3, lvg.v, &lvg.v_bufferid);
	ret = !ret | !load_vbuffer(1, 3, lvg.vn, &lvg.vn_bufferid);
	ret = !ret | !load_vbuffer(2, 2, lvg.vt, &lvg.vt_bufferid);
	ret = !ret | !load_vbuffer(3, 3, lvg.tan, &lvg.tan_bufferid);
	ret = !ret | !load_vbuffer(4, 3, lvg.bitan, &lvg.bitan_bufferid);
	printf("Total Size %i, Buffer ID %i\n", lvg.v.total_size, lvg.v_bufferid);
	printf("Total Size %i, Buffer ID %i\n", lvg.vn.total_size, lvg.vn_bufferid);
	printf("Total Size %i, Buffer ID %i\n", lvg.vt.total_size, lvg.vt_bufferid);
	printf("Total Size %i, Buffer ID %i\n", lvg.tan.total_size, lvg.tan_bufferid);
	printf("Total Size %i, Buffer ID %i\n", lvg.bitan.total_size, lvg.bitan_bufferid);
	glBindVertexArray(0);
	*vg = lvg;
	return (ret);
}

void		load_mesh(t_engine *engine, int mesh_index)
{
	GLuint	i;
	t_mesh	*mesh;

	i = 0;
	mesh = ezarray_get_index(engine->meshes, mesh_index);
	if (!mesh)
		return;
	while (i < mesh->vgroups.length)
	{
		load_vgroup(ezarray_get_index(mesh->vgroups, i));
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
	{
		printf("uniform name : %s | uniform type : %i\n", variable->name.tostring, variable->type);
		glUniform3fv(variable->loc, 1, ((float*)value));
	}
	else if (variable->type == GL_FLOAT_MAT4)
	{
		printf("uniform name : %s | uniform type : %i\n", variable->name.tostring, variable->type);
		glUniformMatrix4fv(variable->loc, 1, GL_FALSE, ((float*)value));
	}
	else if ((variable->type == GL_INT || variable->type == GL_BOOL || variable->type == GL_SAMPLER_2D || variable->type == GL_SAMPLER_CUBE))
	{
		printf("uniform name : %s | uniform type : %i\n", variable->name.tostring, variable->type);
		glUniform1i(variable->loc, *((int*)value));
	}
	else if (variable->type == GL_UNSIGNED_INT)
	{
		printf("uniform name : %s | uniform type : %i\n", variable->name.tostring, variable->type);
		glUniform1ui(variable->loc, *((int*)value));
	}
	else if (variable->type == GL_FLOAT)
	{
		printf("uniform name : %s | uniform type : %i\n", variable->name.tostring, variable->type);
		glUniform1f(variable->loc, *((float*)value));
	}
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

/*void	set_shader_uniform(t_shader *shader, char *name, void *value)
{
	ARRAY				uniforms;
	t_shadervariable	v;
	unsigned			i;
	ULL					h;

	uniforms = shader->uniforms;
	i = 0;
	h = hash((unsigned char*)name);
	while (i < uniforms.length)
	{
		v = *(t_shadervariable*)ezarray_get_index(uniforms, i);
		if (v.type == GL_FLOAT_VEC3 && v.id == h)
		{
			printf("uniform name : %s | uniform type : %i\n", v.name.tostring, v.type);
			glUniform3fv(v.loc, 1, ((float*)value));
			break ;
		}
		else if (v.type == GL_FLOAT_MAT4 && v.id == h)
		{
			printf("uniform name : %s | uniform type : %i\n", v.name.tostring, v.type);
			glUniformMatrix4fv(v.loc, 1, GL_FALSE, ((float*)value));
			break ;
		}
		else if ((v.type == GL_INT || v.type == GL_BOOL || v.type == GL_SAMPLER_2D || v.type == GL_SAMPLER_CUBE) && v.id == h)
		{
			printf("uniform name : %s | uniform type : %i\n", v.name.tostring, v.type);
			glUniform1i(v.loc, *((int*)value));
			break ;
		}
		else if (v.type == GL_UNSIGNED_INT && v.id == h)
		{
			printf("uniform name : %s | uniform type : %i\n", v.name.tostring, v.type);
			glUniform1ui(v.loc, *((int*)value));
			break ;
		}
		else if (v.type == GL_FLOAT && v.id == h)
		{
			printf("uniform name : %s | uniform type : %i\n", v.name.tostring, v.type);
			glUniform1f(v.loc, *((float*)value));
			break ;
		}
		i++;
	}
}*/

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
	glTexImage2D(target, 0, internal_format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
	glGenerateMipmap(target);
	glBindTexture(target, 0);
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

/*void	set_shader_texture(t_shader *shader, char *name, t_texture *texture, GLenum texture_unit)
{
	glActiveTexture(texture_unit);
	glBindTexture(texture->target, texture->id_ogl);
	texture_unit -= GL_TEXTURE0;
	set_shader_uniform(shader, name, &texture_unit);
}*/

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
	glBindTexture(texture->target, 0);
	return (texture->id_ogl);
}

/*void	set_shader_textures(t_engine *engine, int shader_index, int mesh_index, int vgroup_index)
{
	t_texture	*texture[4];
	t_material	*mtl;
	t_vgroup	*vgroup;
	t_mesh		*mesh;
	GLint		use_texture;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	vgroup = ezarray_get_index(mesh->vgroups, vgroup_index);
	texture[0] = ezarray_get_index(engine->textures, mtl->data.texture_albedo);
	if (texture[0] && !texture[0]->loaded)
			load_texture(texture[0], GL_TEXTURE_2D);
	texture[1] = ezarray_get_index(engine->textures, mtl->data.texture_normal);
	if (texture[1] && !texture[1]->loaded)
			load_texture(texture[1], GL_TEXTURE_2D);
	texture[2] = ezarray_get_index(engine->textures, mtl->data.texture_roughness);
	if (texture[2] && !texture[2]->loaded)
			load_texture(texture[2], GL_TEXTURE_2D);
	texture[3] = ezarray_get_index(engine->textures, mtl->data.texture_metallic);
	if (texture[3] && !texture[3]->loaded)
			load_texture(texture[3], GL_TEXTURE_2D);
	if ((use_texture = texture[0] ? 1 : 0))
		set_shader_texture(engine, shader_index, vgroup->shader_texture_albedo, texture[0], GL_TEXTURE0 + 2);
	set_shader_uniform(engine, shader_index, vgroup->shader_use_texture_albedo, &use_texture);
	if ((use_texture = texture[1] ? 1 : 0))
		set_shader_texture(engine, shader_index, vgroup->shader_texture_normal, texture[1], GL_TEXTURE0 + 3);
	set_shader_uniform(engine, shader_index, vgroup->shader_use_texture_normal, &use_texture);
	if ((use_texture = texture[2] ? 1 : 0))
		set_shader_texture(engine, shader_index, vgroup->shader_texture_roughness, texture[2], GL_TEXTURE0 + 4);
	set_shader_uniform(engine, shader_index, vgroup->shader_use_texture_roughness, &use_texture);
	if ((use_texture = texture[3] ? 1 : 0))
		set_shader_texture(engine, shader_index, vgroup->shader_texture_metallic, texture[3], GL_TEXTURE0 + 5);
	set_shader_uniform(engine, shader_index, vgroup->shader_use_texture_metallic, &use_texture);
}*/

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
	vgroup->in_normalmatrix = get_uniform_index(engine, shader_index, "in_NormalMatrix");
	vgroup->in_albedo = get_uniform_index(engine, shader_index, "in_Albedo");
	vgroup->in_uv_scale = get_uniform_index(engine, shader_index, "in_UVScale");
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

/*void	render_mesh(t_engine *engine, int mesh_index)
{
	unsigned	i;
	t_vgroup	vg;
	t_camera	cam;
	t_mesh		m;

	m = *((t_mesh*)ezarray_get_index(engine->meshes, mesh_index));
	glClearColor(0.46f, 0.53f, 0.6f, 1.0f);
	t_shader	shader = load_shaders("/src/shaders/default.vert", "/src/shaders/default.frag");
	//GLuint	prog = load_shaders("/src/shaders/point.vert", "/src/shaders/point.frag");
	cam.position = new_vec3(-1, 0.9, 1);
	cam.target_index = create_transform(engine, new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	cam.view = mat4_lookat(cam.position, new_vec3(0, 0.5, 0), UP);
	cam.projection = mat4_perspective(45, (float)1024 / (float)768, 0.1, 100);
	t_transform	t;

	t = new_transform(new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1), UP);
	transform_update(&t);
	MAT4	mvp;

	mvp = mat4_combine(cam.projection, cam.view, t.transform);
	glUseProgram(shader.program);
	glEnable(GL_PROGRAM_POINT_SIZE);
	set_shader_uniform(&shader, "in_Transform", &mvp.m[0]);
	MAT4	normal_matrix = mat4_transpose(mat4_inverse(t.transform));
	set_shader_uniform(&shader, "in_NormalMatrix", &normal_matrix.m[0]);
	set_shader_uniform(&shader, "in_CamPos", &cam.position);
	i = 0;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ARRAY mtllib;
	mtllib = engine->materials;



	while (i < m.vgroups.length)
	{
		glUseProgram(shader.program);
		vg = *(t_vgroup*)ezarray_get_index(m.vgroups, i);
		t_material *mtl = ezarray_get_index(mtllib, vg.mtl_index);
		set_material_textures(engine, &shader, mtl);
		set_shader_texture(&shader, "in_Texture_Env", &env, GL_TEXTURE0);
		set_shader_texture(&shader, "in_Texture_Env_Spec", &env_spec, GL_TEXTURE0 + 1);
		set_shader_uniform(&shader, "in_Albedo", &mtl->data.albedo);
		set_shader_uniform(&shader, "in_UVScale", &mtl->data.uv_scale);
		set_shader_uniform(&shader, "in_Roughness", &mtl->data.roughness);
		set_shader_uniform(&shader, "in_Metallic", &mtl->data.metallic);
		set_shader_uniform(&shader, "in_Refraction", &mtl->data.refraction);
		set_shader_uniform(&shader, "in_Alpha", &mtl->data.alpha);
		set_shader_uniform(&shader, "in_Parallax", &mtl->data.parallax);
		glBindVertexArray(vg.v_arrayid);
		glDrawArrays(GL_TRIANGLES, 0, vg.v.length);
		i++;
	}
	glBindVertexArray(0);
}*/

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

void	render_vgroup(t_engine *engine, int mesh_index, int vgroup_index)
{
	t_mesh		*mesh;
	t_vgroup	*vgroup;
	t_shader	*shader;
	t_material	*material;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	vgroup = ezarray_get_index(mesh->vgroups, vgroup_index);
	if (!vgroup)
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
	transform = mat4_combine(engine->camera.projection, engine->camera.view, t->transform);
	MAT4	normal_matrix = mat4_transpose(mat4_inverse(t->transform));
	glUseProgram(shader->program);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	set_shader_textures(engine, vgroup, material);
	//set_shader_uniform(&shader, "in_CamPos", &cam.position);
	//set_shader_textures(engine, vgroup->shader_index, mesh_index, vgroup_index);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_campos, &engine->camera.position);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_normalmatrix, &normal_matrix);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_transform, &transform);

	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_albedo, &material->data.albedo);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_uv_scale, &material->data.uv_scale);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_roughness, &material->data.roughness);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_metallic, &material->data.metallic);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_refraction, &material->data.refraction);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_alpha, &material->data.alpha);
	set_shader_uniform(engine, vgroup->shader_index, vgroup->in_parallax, &material->data.parallax);
	/*t_material *mtl = ezarray_get_index(mtllib, vgroup->mtl_index);
	set_material_textures(engine, &shader, mtl);
	set_shader_texture(&shader, "in_Texture_Env", &env, GL_TEXTURE0);
	set_shader_texture(&shader, "in_Texture_Env_Spec", &env_spec, GL_TEXTURE0 + 1);*/
	glBindVertexArray(vgroup->v_arrayid);
	glDrawArrays(GL_TRIANGLES, 0, vgroup->v.length);
	glBindVertexArray(0);
	glUseProgram(0);
}

void	render_mesh(t_engine *engine, int mesh_index)
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
		render_vgroup(engine, mesh_index, vgroup_index);
		vgroup_index++;
	}
}

void	update_camera(t_engine *engine, int camera_index)
{
	t_transform *camera_target;

	camera_target = ezarray_get_index(engine->transforms, engine->camera.target_index);
	engine->camera.view = mat4_lookat(engine->camera.position, camera_target->position, UP);
	engine->camera.projection = mat4_perspective(60, (float)WIDTH / (float)HEIGHT, 0.1, 10000);
	(void)camera_index;
}

void	camera_set_target(t_engine *engine, int camera_index, int mesh_index)
{
	engine->camera.target_index = ((t_mesh*)ezarray_get_index(engine->meshes, mesh_index))->transform_index;
	(void)camera_index;
}

void	camera_set_position(t_engine *engine, int camera_index, VEC3 position)
{
	engine->camera.position = position;
	(void)camera_index;
}

void	render_scene(t_engine *engine)
{
	unsigned	mesh_index;

	mesh_index = 0;
	glClearColor(0.46f, 0.53f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	printf("rendering\n");
	update_camera(engine, 0);
	while (mesh_index < engine->meshes.length)
	{
		render_mesh(engine, mesh_index);
		mesh_index++;
	}
}

void	center_vgroup(t_engine *engine, int mesh_index, int vgroup_index)
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

void	center_mesh(t_engine *engine, int mesh_index)
{
	t_mesh		*mesh;
	unsigned	vgroup_index;

	mesh = ezarray_get_index(engine->meshes, mesh_index);
	if (!mesh)
		return ;
	vgroup_index = 0;
	while (vgroup_index < mesh->vgroups.length)
	{
		center_vgroup(engine, mesh_index, vgroup_index);
		vgroup_index++;
	}
	mesh->bounding_box.center = new_vec3(0, 0, 0);
	mesh->bounding_box.min = vec3_sub(mesh->bounding_box.min, mesh->bounding_box.center);
	mesh->bounding_box.max = vec3_sub(mesh->bounding_box.max, mesh->bounding_box.center);
}

void load_env(t_engine *engine)
{
	int X0 = load_bmp(engine, "./res/skybox/cloudtop/X+.bmp"), X1 = load_bmp(engine, "./res/skybox/cloudtop/X-.bmp"),
	Y0 = load_bmp(engine, "./res/skybox/cloudtop/Y-.bmp"), Y1 = load_bmp(engine, "./res/skybox/cloudtop/Y+.bmp"),
	Z0 = load_bmp(engine, "./res/skybox/cloudtop/Z+.bmp"), Z1 = load_bmp(engine, "./res/skybox/cloudtop/Z-.bmp");
	int X0_spec = load_bmp(engine, "./res/skybox/cloudtop/X+_spec.bmp"), X1_spec = load_bmp(engine, "./res/skybox/cloudtop/X-_spec.bmp"),
	Y0_spec = load_bmp(engine, "./res/skybox/cloudtop/Y-_spec.bmp"), Y1_spec = load_bmp(engine, "./res/skybox/cloudtop/Y+_spec.bmp"),
	Z0_spec = load_bmp(engine, "./res/skybox/cloudtop/Z+_spec.bmp"), Z1_spec = load_bmp(engine, "./res/skybox/cloudtop/Z-_spec.bmp");


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
	//return (engine->textures.length - 2);
}

int main(int argc, char *argv[])
{
	t_engine	*e;

	if (argc < 2)
		return (0);
	e = init_engine();
	init_window(e, "Scope", WIDTH, HEIGHT);
	printf("%s\n", glGetString(GL_VERSION));
	load_env(e);
	int obj = load_obj(e, argv[1]);
	center_mesh(e, obj);
	int shader = load_shaders(e, "default", "/src/shaders/default.vert", "/src/shaders/default.frag");
	camera_set_target(e, 0, obj);
	camera_set_position(e, 0, new_vec3(-500, 4, 500));
	load_mesh(e, obj);
	assign_shader_to_mesh(e, obj, shader);
	render_scene(e);
	//render_mesh(e, obj);
	glDisableVertexAttribArray(0);
	SDL_GL_SwapWindow(e->window->sdl_window);
	sleep(5);
	return (argc + argv[0][0]);
}