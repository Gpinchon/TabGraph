/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   present.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 16:28:23 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 18:28:30 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

static void	render_bind_textures(int shader)
{
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Color"),
	framebuffer_get_attachement(window_get()->render_buffer, 0), GL_TEXTURE0);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Bright"),
	framebuffer_get_attachement(window_get()->render_buffer, 1), GL_TEXTURE1);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Normal"),
	framebuffer_get_attachement(window_get()->render_buffer, 2), GL_TEXTURE2);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Position"),
	framebuffer_get_attachement(window_get()->render_buffer, 3), GL_TEXTURE3);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Depth"),
		framebuffer_get_depth(window_get()->render_buffer), GL_TEXTURE4);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Env"),
		engine_get()->env, GL_TEXTURE5);
	shader_bind_texture(shader,
		shader_get_uniform_index(shader, "in_Texture_Env_Spec"),
		engine_get()->env_spec, GL_TEXTURE6);
}

void		render_present(int camera_index)
{
	int			shader;
	t_camera	*camera;
	MAT4		matrix;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, window_get_size().x, window_get_size().y);
	shader = framebuffer_get_shader(window_get()->render_buffer);
	texture_generate_mipmap(
		framebuffer_get_attachement(window_get()->render_buffer, 0));
	shader_use(shader);
	render_bind_textures(shader);
	camera = ezarray_get_index(engine_get()->cameras, camera_index);
	matrix = mat4_inverse(camera->view);
	shader_set_mat4(shader,
		shader_get_uniform_index(shader, "in_InvViewMatrix"),
		matrix);
	matrix = mat4_inverse(camera->projection);
	shader_set_mat4(shader,
		shader_get_uniform_index(shader, "in_InvProjMatrix"),
		matrix);
	glBindVertexArray(display_quad_get());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
}
