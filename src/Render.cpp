/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:42:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/27 19:24:12 by gpinchon         ###   ########.fr       */
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
	vao->add_buffer(GL_FLOAT, 2, quad);
	return (vao);
}

#include "parser/GLSL.hpp"

void	Render::scene()
{
	/*
	** TODO :
	** 	- CLEANUP CODE
	** 	- MANAGE TRANSPARENT OBJECTS
	*/
	glClearColor(0, 0, 0, 0);
	Window::render_buffer().bind();
	glClear(Window::clear_mask());
	glDepthFunc(GL_LESS);
	Renderable *node;
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderOpaque);
	}
	glDepthFunc(GL_ALWAYS);
	
	static auto	temp_buffer = Framebuffer::create("temp_buffer", Window::internal_resolution(), nullptr, 7, 1);
	static auto	back_buffer = Framebuffer::create("temp_buffer", Window::internal_resolution(), nullptr, 1, 1);
	static auto	passthrough_shader = GLSL::parse("passthrough", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/passthrough.frag");
	static auto	lighting_shader = GLSL::parse("lighting", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/lighting.frag");
	static auto	tlighting_shader = GLSL::parse("lighting", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/lighting_transparent.frag");
	static auto	presentShader = GLSL::parse("present", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/present.frag");
	
	temp_buffer->resize(Window::internal_resolution());
	for (Shader *shader : post_treatments)
	{
		/*
		** APPLY POST-TREATMENT
		*/
		temp_buffer->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader->use();
		shader->bind_texture("in_Texture_Albedo", Window::render_buffer().attachement(0), GL_TEXTURE0); // Albedo;
		shader->bind_texture("in_Texture_Fresnel", Window::render_buffer().attachement(1), GL_TEXTURE1); // Fresnel;
		shader->bind_texture("in_Texture_Emitting", Window::render_buffer().attachement(2), GL_TEXTURE2); // Emitting;
		shader->bind_texture("in_Texture_Material_Values", Window::render_buffer().attachement(3), GL_TEXTURE3); // Material_Values -> Roughness, Metallic
		shader->bind_texture("in_Texture_BRDF", Window::render_buffer().attachement(4), GL_TEXTURE4); // BRDF
		shader->bind_texture("in_Texture_Normal", Window::render_buffer().attachement(5), GL_TEXTURE5); // Normal;
		shader->bind_texture("in_Texture_Position", Window::render_buffer().attachement(6), GL_TEXTURE6); // Position;
		shader->bind_texture("in_Texture_Depth", Window::render_buffer().depth(), GL_TEXTURE7);
		shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
		shader->bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE9);
		Render::display_quad()->draw();
		shader->use(false);

		// COPY RESULT TO MAIN FRAMEBUFFER
		Window::render_buffer().bind();
		passthrough_shader->use();
		passthrough_shader->bind_texture("in_Texture_Albedo", temp_buffer->attachement(0), GL_TEXTURE0);
		passthrough_shader->bind_texture("in_Texture_Fresnel", temp_buffer->attachement(1), GL_TEXTURE1);
		passthrough_shader->bind_texture("in_Texture_Emitting", temp_buffer->attachement(2), GL_TEXTURE2);
		passthrough_shader->bind_texture("in_Texture_Material_Values", temp_buffer->attachement(3), GL_TEXTURE3);
		passthrough_shader->bind_texture("in_Texture_BRDF", temp_buffer->attachement(4), GL_TEXTURE4);
		passthrough_shader->bind_texture("in_Texture_Normal", temp_buffer->attachement(5), GL_TEXTURE5);
		passthrough_shader->bind_texture("in_Texture_Position", temp_buffer->attachement(6), GL_TEXTURE6);
		passthrough_shader->bind_texture("in_Texture_Depth", temp_buffer->depth(), GL_TEXTURE7);
		passthrough_shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
		passthrough_shader->bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE9);
		Render::display_quad()->draw();
		passthrough_shader->use(false);
	}

	auto	InvViewMatrix = mat4_inverse(Engine::current_camera()->view);
	auto	InvProjMatrix = mat4_inverse(Engine::current_camera()->projection);

	// APPLY LIGHTING SHADER
	// OUTPUT : out_Color, out_Brightness
	temp_buffer->bind();
	lighting_shader->use();
	lighting_shader->set_uniform("in_CamPos", Engine::current_camera()->position());
	lighting_shader->set_uniform("in_InvViewMatrix", InvViewMatrix);
	lighting_shader->set_uniform("in_InvProjMatrix", InvProjMatrix);
	lighting_shader->bind_texture("in_Texture_Albedo", Window::render_buffer().attachement(0), GL_TEXTURE0); // Albedo;
	lighting_shader->bind_texture("in_Texture_Fresnel", Window::render_buffer().attachement(1), GL_TEXTURE1); // Fresnel;
	lighting_shader->bind_texture("in_Texture_Emitting", Window::render_buffer().attachement(2), GL_TEXTURE2); // Emitting;
	lighting_shader->bind_texture("in_Texture_Material_Values", Window::render_buffer().attachement(3), GL_TEXTURE3); // Material_Values -> Roughness, Metallic
	lighting_shader->bind_texture("in_Texture_BRDF", Window::render_buffer().attachement(4), GL_TEXTURE4); // BRDF
	lighting_shader->bind_texture("in_Texture_Normal", Window::render_buffer().attachement(5), GL_TEXTURE5); // Normal;
	lighting_shader->bind_texture("in_Texture_Position", Window::render_buffer().attachement(6), GL_TEXTURE6); // Position;
	lighting_shader->bind_texture("in_Texture_Depth", Window::render_buffer().depth(), GL_TEXTURE7);
	lighting_shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
	lighting_shader->bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE9);
	Render::display_quad()->draw();
	lighting_shader->use(false);
	
	// GENERATE BLOOM FROM out_Brightness
	temp_buffer->attachement(1)->blur(BLOOMPASS, 2.5);

	// PRESENT RESULT (out_Color + out_Brightness)	
	//Framebuffer::bind_default();
	back_buffer->bind();
	presentShader->use();
	presentShader->bind_texture("in_Texture_Color", temp_buffer->attachement(0), GL_TEXTURE0);
	presentShader->bind_texture("in_Texture_Emitting", temp_buffer->attachement(1), GL_TEXTURE1);
	presentShader->bind_texture("in_Texture_Depth", temp_buffer->depth(), GL_TEXTURE2);
	Render::display_quad()->draw();
	presentShader->use(false);

	Window::render_buffer().bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderTransparent);
	}
	glDepthFunc(GL_ALWAYS);

	temp_buffer->bind();
	tlighting_shader->use();
	tlighting_shader->set_uniform("in_CamPos", Engine::current_camera()->position());
	//tlighting_shader->set_uniform("in_InvViewMatrix", InvViewMatrix);
	//tlighting_shader->set_uniform("in_InvProjMatrix", InvProjMatrix);
	tlighting_shader->bind_texture("in_Texture_Albedo", Window::render_buffer().attachement(0), GL_TEXTURE0); // Albedo;
	tlighting_shader->bind_texture("in_Texture_Fresnel", Window::render_buffer().attachement(1), GL_TEXTURE1); // Fresnel;
	tlighting_shader->bind_texture("in_Texture_Emitting", Window::render_buffer().attachement(2), GL_TEXTURE2); // Emitting;
	tlighting_shader->bind_texture("in_Texture_Material_Values", Window::render_buffer().attachement(3), GL_TEXTURE3); // Material_Values -> Roughness, Metallic
	tlighting_shader->bind_texture("in_Texture_BRDF", Window::render_buffer().attachement(4), GL_TEXTURE4); // BRDF
	tlighting_shader->bind_texture("in_Texture_Normal", Window::render_buffer().attachement(5), GL_TEXTURE5); // Normal;
	tlighting_shader->bind_texture("in_Texture_Position", Window::render_buffer().attachement(6), GL_TEXTURE6); // Position;
	tlighting_shader->bind_texture("in_Texture_Depth", Window::render_buffer().depth(), GL_TEXTURE7);
	tlighting_shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
	tlighting_shader->bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE9);
	tlighting_shader->bind_texture("in_Back_Buffer", back_buffer->attachement(0), GL_TEXTURE10);
	Render::display_quad()->draw();
	tlighting_shader->use(false);

	back_buffer->bind();
	presentShader->use();
	presentShader->bind_texture("in_Texture_Color", temp_buffer->attachement(0), GL_TEXTURE0);
	presentShader->bind_texture("in_Texture_Emitting", temp_buffer->attachement(1), GL_TEXTURE1);
	presentShader->bind_texture("in_Texture_Depth", temp_buffer->depth(), GL_TEXTURE2);
	Render::display_quad()->draw();
	presentShader->use(false);

	Framebuffer::bind_default();
	presentShader->use();
	presentShader->bind_texture("in_Texture_Color", back_buffer->attachement(0), GL_TEXTURE0);
	presentShader->bind_texture("in_Texture_Emitting", nullptr, GL_TEXTURE1);
	presentShader->bind_texture("in_Texture_Depth", temp_buffer->depth(), GL_TEXTURE2);
	Render::display_quad()->draw();
	presentShader->use(false);
	/*for (Shader *shader : post_treatments)
	{
		temp_buffer->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader->use();
		shader->bind_texture("in_Texture_Albedo", Window::render_buffer().attachement(0), GL_TEXTURE0); // Albedo;
		shader->bind_texture("in_Texture_Fresnel", Window::render_buffer().attachement(1), GL_TEXTURE1); // Fresnel;
		shader->bind_texture("in_Texture_Emitting", Window::render_buffer().attachement(2), GL_TEXTURE2); // Emitting;
		shader->bind_texture("in_Texture_Material_Values", Window::render_buffer().attachement(3), GL_TEXTURE3); // Material_Values -> Roughness, Metallic
		shader->bind_texture("in_Texture_BRDF", Window::render_buffer().attachement(4), GL_TEXTURE4); // BRDF
		shader->bind_texture("in_Texture_Normal", Window::render_buffer().attachement(5), GL_TEXTURE5); // Normal;
		shader->bind_texture("in_Texture_Position", Window::render_buffer().attachement(6), GL_TEXTURE6); // Position;
		shader->bind_texture("in_Texture_Depth", Window::render_buffer().depth(), GL_TEXTURE7);
		shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
		shader->bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE9);
		Render::display_quad()->draw();
		shader->use(false);

		Window::render_buffer().bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		passthrough_shader->use();
		passthrough_shader->bind_texture("in_Texture_Albedo", temp_buffer->attachement(0), GL_TEXTURE0);
		passthrough_shader->bind_texture("in_Texture_Fresnel", temp_buffer->attachement(1), GL_TEXTURE1);
		passthrough_shader->bind_texture("in_Texture_Emitting", temp_buffer->attachement(2), GL_TEXTURE2);
		passthrough_shader->bind_texture("in_Texture_Material_Values", temp_buffer->attachement(3), GL_TEXTURE3);
		passthrough_shader->bind_texture("in_Texture_BRDF", temp_buffer->attachement(4), GL_TEXTURE4);
		passthrough_shader->bind_texture("in_Texture_Normal", temp_buffer->attachement(5), GL_TEXTURE5);
		passthrough_shader->bind_texture("in_Texture_Position", temp_buffer->attachement(6), GL_TEXTURE6);
		passthrough_shader->bind_texture("in_Texture_Depth", temp_buffer->depth(), GL_TEXTURE7);
		passthrough_shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
		passthrough_shader->bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE9);
		Render::display_quad()->draw();
		passthrough_shader->use(false);
	}*/
}

void	Render::add_post_treatment(Shader *shader)
{
	if (shader != nullptr)
		post_treatments.insert(shader);
}

void	Render::remove_post_treatment(Shader *shader)
{
	if (shader != nullptr)
		post_treatments.erase(shader);
}

void	Render::bind_textures(Shader *shader)
{
	shader->bind_texture("in_Texture_Albedo", Window::render_buffer().attachement(0), GL_TEXTURE0); // Albedo;
	shader->bind_texture("in_Texture_Fresnel", Window::render_buffer().attachement(1), GL_TEXTURE1); // Fresnel;
	shader->bind_texture("in_Texture_Emitting", Window::render_buffer().attachement(2), GL_TEXTURE2); // Emitting;
	shader->bind_texture("in_Texture_Material_Values", Window::render_buffer().attachement(3), GL_TEXTURE3); // Material_Values -> Roughness, Metallic
	shader->bind_texture("in_Texture_BRDF", Window::render_buffer().attachement(4), GL_TEXTURE4); // BRDF
	shader->bind_texture("in_Texture_Normal", Window::render_buffer().attachement(5), GL_TEXTURE5); // Normal;
	shader->bind_texture("in_Texture_Position", Window::render_buffer().attachement(6), GL_TEXTURE6); // Position;
	shader->bind_texture("in_Texture_Depth", Window::render_buffer().depth(), GL_TEXTURE7);
	shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
	shader->bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE9);
}

void		Render::present()
{
	MAT4		matrix{{0}};

	if (Engine::current_camera() == nullptr) {
		return ;
	}
	Window::render_buffer().attachement(2)->blur(BLOOMPASS, 2.5);
	//Window::render_buffer().attachement(6).blur(1, 2.5);
	Framebuffer::bind_default();
	glDisable(GL_DEPTH_TEST);
	auto	shader = Window::render_buffer().shader();
	//Window::render_buffer().attachement(0).generate_mipmap();
	shader->use();
	Render::bind_textures(shader);
	shader->set_uniform("in_CamPos", Engine::current_camera()->position());
	matrix = mat4_inverse(Engine::current_camera()->view);
	shader->set_uniform("in_InvViewMatrix", matrix);
	matrix = mat4_inverse(Engine::current_camera()->projection);
	shader->set_uniform("in_InvProjMatrix", matrix);
	Render::display_quad()->draw();
	shader->use(false);
}
