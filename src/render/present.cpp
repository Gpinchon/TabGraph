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

#include "scop.hpp"

static void	render_bind_textures(Shader &shader)
{
	shader.bind_texture("in_Texture_Color",
	&Window::render_buffer().attachement(0), GL_TEXTURE0);
	shader.bind_texture("in_Texture_Bright",
	&Window::render_buffer().attachement(1), GL_TEXTURE1);
	shader.bind_texture("in_Texture_Normal",
	&Window::render_buffer().attachement(2), GL_TEXTURE2);
	shader.bind_texture("in_Texture_Position",
	&Window::render_buffer().attachement(3), GL_TEXTURE3);
	shader.bind_texture("in_Texture_Depth",
	&Window::render_buffer().depth(), GL_TEXTURE4);
	shader.bind_texture("in_Texture_Env",
		Engine::current_environment()->diffuse, GL_TEXTURE5);
	shader.bind_texture("in_Texture_Env_Spec",
		Engine::current_environment()->brdf, GL_TEXTURE6);
}

void		render_present()
{
	MAT4		matrix;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, Window::size().x, Window::size().y);
	Shader	&shader = Window::render_buffer().shader();
	Window::render_buffer().attachement(0).generate_mipmap();
	shader.use();
	render_bind_textures(shader);
	matrix = mat4_inverse(Engine::current_camera()->view);
	shader.set_uniform("in_InvViewMatrix", matrix);
	matrix = mat4_inverse(Engine::current_camera()->projection);
	shader.set_uniform("in_InvProjMatrix", matrix);
	glBindVertexArray(display_quad_get());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
}
