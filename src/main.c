/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/01/19 23:21:50 by gpinchon         ###   ########.fr       */
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
	return (e->transforms.length);
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

int		load_mesh(t_mesh *m)
{
	GLuint	i;
	int		ret;
	t_mesh	lm;

	i = 0;
	lm = *m;
	ret = 0;
	while (i < lm.vgroups.length)
	{
		ret = !ret && !load_vgroup(ezarray_get_index(lm.vgroups, i));
		i++;
	}
	*m = lm;
	return (ret);
}

void	set_shader_uniform(t_shader *shader, char *name, void *value)
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
}

GLuint			load_texture(t_texture *texture, GLenum target)
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

void	set_shader_texture(t_shader *shader, char *name, t_texture *texture, GLenum texture_unit)
{
	glActiveTexture(texture_unit);
	glBindTexture(texture->target, texture->id_ogl);
	texture_unit -= GL_TEXTURE0;
	set_shader_uniform(shader, name, &texture_unit);
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

void	set_material_textures(t_engine *engine, t_shader *shader, t_material *mtl)
{
	t_texture	*texture[4];
	GLint		use_texture;

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
		set_shader_texture(shader, "in_Texture_Albedo", texture[0], GL_TEXTURE0 + 2);
	set_shader_uniform(shader, "in_Use_Texture_Albedo", &use_texture);
	if ((use_texture = texture[1] ? 1 : 0))
		set_shader_texture(shader, "in_Texture_Normal", texture[1], GL_TEXTURE0 + 3);
	set_shader_uniform(shader, "in_Use_Texture_Normal", &use_texture);
	if ((use_texture = texture[2] ? 1 : 0))
		set_shader_texture(shader, "in_Texture_Roughness", texture[2], GL_TEXTURE0 + 4);
	set_shader_uniform(shader, "in_Use_Texture_Roughness", &use_texture);
	if ((use_texture = texture[3] ? 1 : 0))
		set_shader_texture(shader, "in_Texture_Metallic", texture[3], GL_TEXTURE0 + 5);
	set_shader_uniform(shader, "in_Use_Texture_Metallic", &use_texture);
}

void	render_mesh(t_engine *engine, t_mesh m)
{
	unsigned	i;
	t_vgroup	vg;
	t_camera	cam;

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
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ARRAY mtllib;
	mtllib = engine->materials;

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

	t_texture	env_spec;
	env_spec.target = GL_TEXTURE_CUBE_MAP;
	generate_texture(&env_spec);
	glBindTexture(env_spec.target, env_spec.id_ogl);
	assign_texture(ezarray_get_index(engine->textures, X0_spec), env_spec.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	assign_texture(ezarray_get_index(engine->textures, X1_spec), env_spec.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	assign_texture(ezarray_get_index(engine->textures, Y0_spec), env_spec.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	assign_texture(ezarray_get_index(engine->textures, Y1_spec), env_spec.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	assign_texture(ezarray_get_index(engine->textures, Z0_spec), env_spec.id_ogl, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	assign_texture(ezarray_get_index(engine->textures, Z1_spec), env_spec.id_ogl, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	glGenerateMipmap(env_spec.target);
	glBindTexture(env_spec.target, 0);

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
}

int main(int argc, char *argv[])
{
	t_engine	*e;

	if (argc < 2)
		return (0);
	e = init_engine();
	init_window(e, "Scope", 1024, 768);
	load_obj(e, argv[1]);
	printf("%s\n", glGetString(GL_VERSION));
	/*static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
	};*/
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Draw the triangle !
	 // Starting from vertex 0; 3 vertices total -> 1 triangle
	load_mesh(ezarray_get_index(e->meshes, 0));
	render_mesh(e, *(t_mesh*)ezarray_get_index(e->meshes, 0));
	glDisableVertexAttribArray(0);
	SDL_GL_SwapWindow(e->window->sdl_window);
	sleep(5);
	return (argc + argv[0][0]);
}