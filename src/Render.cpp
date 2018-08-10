/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:42:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/10 11:19:36 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"
#include "Engine.hpp"
#include "Renderable.hpp"
#include "Camera.hpp"
#include "Cubemap.hpp"
#include "Framebuffer.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include <vector>

/*
** quad is a singleton
*/

const VertexArray	*Render::display_quad()
{
	static VertexArray	*vao = nullptr;
	if (vao != nullptr) {
		return (vao);
	}
	vao = VertexArray::create(4, GL_TRIANGLE_STRIP);
	std::vector<float>	quad(8);
	quad.at(0) = -1.0f;
	quad.at(1) = -1.0f;
	quad.at(2) = 1.0f;
	quad.at(3) = -1.0f;
	quad.at(4) = -1.0f;
	quad.at(5) = 1.0f;
	quad.at(6) = 1.0f;
	quad.at(7) = 1.0f;
	vao->add_buffer(0, GL_FLOAT, 2, quad);
	return (vao);
}

void	Render::scene()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Renderable *node;
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render();
	}
}

void	Render::bind_textures()
{
	auto	&shader = Window::render_buffer().shader();
	shader.bind_texture("in_Texture_Albedo", &Window::render_buffer().attachement(0), GL_TEXTURE0); // Albedo;
	shader.bind_texture("in_Texture_Fresnel", &Window::render_buffer().attachement(1), GL_TEXTURE1); // Fresnel;
	shader.bind_texture("in_Texture_Emitting", &Window::render_buffer().attachement(2), GL_TEXTURE2); // Emitting;
	shader.bind_texture("in_Texture_Material_Values", &Window::render_buffer().attachement(3), GL_TEXTURE3); // Material_Values -> Roughness, Metallic
	shader.bind_texture("in_Texture_BRDF", &Window::render_buffer().attachement(4), GL_TEXTURE4); // BRDF
	shader.bind_texture("in_Texture_Normal", &Window::render_buffer().attachement(5), GL_TEXTURE5); // Normal;
	shader.bind_texture("in_Texture_Position", &Window::render_buffer().attachement(6), GL_TEXTURE6); // Position;
	shader.bind_texture("in_Texture_Depth", &Window::render_buffer().depth(), GL_TEXTURE7);
	shader.bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
	shader.bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE9);
}

void		Render::present()
{
	MAT4		matrix{{0}};

	if (Engine::current_camera() == nullptr) {
		return ;
	}
	Window::render_buffer().attachement(2).blur(BLOOMPASS, 2.5);
	//Window::render_buffer().attachement(6).blur(1, 2.5);
	Framebuffer::bind_default();
	glDisable(GL_DEPTH_TEST);
	auto	&shader = Window::render_buffer().shader();
	//Window::render_buffer().attachement(0).generate_mipmap();
	shader.use();
	Render::bind_textures();
	shader.set_uniform("in_CamPos", Engine::current_camera()->position());
	matrix = mat4_inverse(Engine::current_camera()->view);
	shader.set_uniform("in_InvViewMatrix", matrix);
	matrix = mat4_inverse(Engine::current_camera()->projection);
	shader.set_uniform("in_InvProjMatrix", matrix);
	Render::display_quad()->draw();
	shader.use(false);
}
