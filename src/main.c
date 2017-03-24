/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2017/03/24 00:12:16 by gpinchon         ###   ########.fr       */
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

/*
** engine is a singleton
*/

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
	ret = !ret && !load_vbuffer(0, 3, lvg.v, &lvg.v_bufferid);
	ret = !ret && !load_vbuffer(1, 3, lvg.vn, &lvg.vn_bufferid);
	ret = !ret && !load_vbuffer(2, 3, lvg.tan, &lvg.tan_bufferid);
	ret = !ret && !load_vbuffer(3, 2, lvg.vt, &lvg.vt_bufferid);
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

void	render_mesh(t_mesh m)
{
	unsigned	i;
	t_vgroup	vg;
	t_camera	cam;

	GLuint	prog = load_shaders("/src/shaders/default.vert", "/src/shaders/default.frag");
	//GLuint	prog = load_shaders("/src/shaders/point.vert", "/src/shaders/point.frag");
	cam.view = mat4_lookat(new_vec3(4, 3, 3), new_vec3(0, 0, 0), UP);
	cam.projection = mat4_perspective(45, (float)1024 / (float)768, 0.1, 100);
	t_transform	t;

	t = new_transform(new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1), UP);
	transform_update(&t);
	MAT4	mvp;

	mvp = mat4_combine(cam.projection, cam.view, t.transform);
	glUseProgram(prog);
	glEnable(GL_PROGRAM_POINT_SIZE);
	GLuint MatrixID = glGetUniformLocation(prog, "in_Transform");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp.m[0]);
	i = 0;
	while (i < m.vgroups.length)
	{
		vg = *(t_vgroup*)ezarray_get_index(m.vgroups, i);
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
	render_mesh(*(t_mesh*)ezarray_get_index(e->meshes, 0));
	glDisableVertexAttribArray(0);
	SDL_GL_SwapWindow(e->window->sdl_window);
	sleep(5);
	return (argc + argv[0][0]);
}