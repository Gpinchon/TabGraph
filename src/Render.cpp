/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:42:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/03 23:45:54 by gpinchon         ###   ########.fr       */
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

Framebuffer	*create_render_buffer(const std::string &name, const VEC2 &size, Shader *shader)
{
	auto	buffer = Framebuffer::create(name, size, shader, 0, 1);
	buffer->create_attachement(GL_RGBA, GL_RGBA8); // Albedo;
	buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emitting;
	buffer->create_attachement(GL_RGB, GL_RGB8); // Fresnel;
	buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Material_Values -> Roughness, Metallic, Ior
	buffer->create_attachement(GL_RG, GL_RG8); // irradiance
	buffer->create_attachement(GL_RED, GL_R8); //AO
	buffer->create_attachement(GL_RGB, GL_RGBA8_SNORM); // Normal;
	buffer->create_attachement(GL_RGB, GL_RGB16F); // Position;
	buffer->setup_attachements();
	return (buffer);
}

Framebuffer	*create_back_buffer(const std::string &name, const VEC2 &size, Shader *shader)
{
	auto	buffer = Framebuffer::create(name, size, shader, 0, 1);
	buffer->create_attachement(GL_RGBA, GL_RGBA8); // Color;
	buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emitting;
	buffer->setup_attachements();
	return (buffer);
}

void	Render::scene()
{
	static auto	temp_buffer = create_render_buffer("temp_buffer", Window::internal_resolution(), nullptr);
	static auto	temp_buffer1 = create_render_buffer("temp_buffer1", Window::internal_resolution(), nullptr);
	static auto	back_buffer = create_back_buffer("back_buffer", Window::internal_resolution(), nullptr);
	static auto	back_buffer1 = create_back_buffer("back_buffer1", Window::internal_resolution(), nullptr);
	static auto	passthrough_shader = GLSL::parse("passthrough", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/passthrough.frag");
	static auto	lighting_shader = GLSL::parse("lighting", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/lighting.frag");
	static auto	tlighting_shader = GLSL::parse("lighting", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/lighting_transparent.frag");
	static auto	presentShader = GLSL::parse("present", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/present.frag");

	temp_buffer->resize(Window::internal_resolution());
	temp_buffer1->resize(Window::internal_resolution());
	back_buffer->resize(Window::internal_resolution());
	back_buffer1->resize(Window::internal_resolution());

	/*
	** TODO :
	** 	- CLEANUP CODE
	** 	- MANAGE TRANSPARENT OBJECTS
	*/
	glClearColor(0, 0, 0, 0);
	temp_buffer1->bind();
	glClear(Window::clear_mask());
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	Renderable *node;
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderOpaque);
	}
	glDepthFunc(GL_ALWAYS);
	
	for (Shader *shader : post_treatments)
	{
		/*
		** APPLY POST-TREATMENT
		*/
		temp_buffer->bind();
		shader->use();
		shader->bind_texture("in_Texture_Albedo",			temp_buffer1->attachement(0), GL_TEXTURE0);
		shader->bind_texture("in_Texture_Emitting",			temp_buffer1->attachement(1), GL_TEXTURE1);
		shader->bind_texture("in_Texture_Fresnel",			temp_buffer1->attachement(2), GL_TEXTURE2);
		shader->bind_texture("in_Texture_Material_Values",	temp_buffer1->attachement(3), GL_TEXTURE3);
		shader->bind_texture("in_Texture_BRDF",				temp_buffer1->attachement(4), GL_TEXTURE4);
		shader->bind_texture("in_Texture_AO",				temp_buffer1->attachement(5), GL_TEXTURE5);
		shader->bind_texture("in_Texture_Normal",			temp_buffer1->attachement(6), GL_TEXTURE6);
		shader->bind_texture("in_Texture_Position",			temp_buffer1->attachement(7), GL_TEXTURE7);
		shader->bind_texture("in_Texture_Depth",			temp_buffer1->depth(), GL_TEXTURE8);
		shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE9);
		shader->bind_texture("Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE10);
		Render::display_quad()->draw();
		shader->use(false);

		// COPY RESULT TO MAIN FRAMEBUFFER
		temp_buffer1->bind();
		passthrough_shader->use();
		passthrough_shader->bind_texture("in_Buffer0", temp_buffer->attachement(0), GL_TEXTURE0);
		passthrough_shader->bind_texture("in_Buffer1", temp_buffer->attachement(1), GL_TEXTURE1);
		passthrough_shader->bind_texture("in_Buffer2", temp_buffer->attachement(2), GL_TEXTURE2);
		passthrough_shader->bind_texture("in_Buffer3", temp_buffer->attachement(3), GL_TEXTURE3);
		passthrough_shader->bind_texture("in_Buffer4", temp_buffer->attachement(4), GL_TEXTURE4);
		passthrough_shader->bind_texture("in_Buffer5", temp_buffer->attachement(5), GL_TEXTURE5);
		passthrough_shader->bind_texture("in_Buffer6", temp_buffer->attachement(6), GL_TEXTURE6);
		passthrough_shader->bind_texture("in_Buffer7", temp_buffer->attachement(7), GL_TEXTURE7);
		passthrough_shader->bind_texture("in_Texture_Depth", temp_buffer->depth(), GL_TEXTURE8);
		Render::display_quad()->draw();
		passthrough_shader->use(false);
	}

	auto	InvViewMatrix = mat4_inverse(Engine::current_camera()->view);
	auto	InvProjMatrix = mat4_inverse(Engine::current_camera()->projection);

	temp_buffer1->attachement(5)->blur(1, 0.75);

	// APPLY LIGHTING SHADER
	// OUTPUT : out_Color, out_Brightness
	back_buffer->bind();
	lighting_shader->use();
	lighting_shader->set_uniform("in_CamPos", Engine::current_camera()->position());
	lighting_shader->set_uniform("in_InvViewMatrix", InvViewMatrix);
	lighting_shader->set_uniform("in_InvProjMatrix", InvProjMatrix);
	lighting_shader->bind_texture("in_Texture_Albedo",			temp_buffer1->attachement(0), GL_TEXTURE0);
	lighting_shader->bind_texture("in_Texture_Emitting",		temp_buffer1->attachement(1), GL_TEXTURE1);
	lighting_shader->bind_texture("in_Texture_Fresnel",			temp_buffer1->attachement(2), GL_TEXTURE2);
	lighting_shader->bind_texture("in_Texture_Material_Values",	temp_buffer1->attachement(3), GL_TEXTURE3);
	lighting_shader->bind_texture("in_Texture_BRDF",			temp_buffer1->attachement(4), GL_TEXTURE4);
	lighting_shader->bind_texture("in_Texture_AO",				temp_buffer1->attachement(5), GL_TEXTURE5);
	lighting_shader->bind_texture("in_Texture_Normal",			temp_buffer1->attachement(6), GL_TEXTURE6);
	lighting_shader->bind_texture("in_Texture_Position",		temp_buffer1->attachement(7), GL_TEXTURE7);
	lighting_shader->bind_texture("in_Texture_Depth",			temp_buffer1->depth(), GL_TEXTURE8);
	lighting_shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE9);
	lighting_shader->bind_texture("Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE10);
	Render::display_quad()->draw();
	lighting_shader->use(false);

	/*
	** ATTEMPT RENDERING TRANSPARENT OBJECTS
	** WRITE DEPTH FOR FUTUR USE
	*/
	temp_buffer1->bind();
	glDisable(GL_CULL_FACE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	bool	rendered_stuff = false;
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		if (node->render_depth(RenderTransparent))
			rendered_stuff = true;
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if (!rendered_stuff)
	{
		/*
		** NO OBJECTS WERE RENDERED PRESENT IMEDIATLY
		*/
		glDepthFunc(GL_ALWAYS);
		back_buffer->attachement(1)->blur(BLOOMPASS, 3.5);
		Framebuffer::bind_default();
		presentShader->use();
		presentShader->bind_texture("in_Texture_Color", back_buffer->attachement(0), GL_TEXTURE0);
		presentShader->bind_texture("in_Texture_Emitting", back_buffer->attachement(1), GL_TEXTURE1);
		presentShader->bind_texture("in_Texture_Depth", back_buffer->depth(), GL_TEXTURE2);
		Render::display_quad()->draw();
		presentShader->use(false);
		return ;
	}

	/*
	** REWRITE TRANSPARENT OBJECTS
	** WRITE ONLY CLOSEST OBJECTS
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_EQUAL);
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderTransparent);
	}
	glDepthFunc(GL_ALWAYS);

	back_buffer->attachement(0)->generate_mipmap();
	back_buffer->attachement(1)->generate_mipmap();

	back_buffer1->bind();
	tlighting_shader->use();
	tlighting_shader->set_uniform("in_CamPos", Engine::current_camera()->position());
	tlighting_shader->set_uniform("in_ViewMatrix", Engine::current_camera()->view);
	tlighting_shader->set_uniform("in_InvViewMatrix", InvViewMatrix);
	tlighting_shader->set_uniform("in_InvProjMatrix", InvProjMatrix);
	tlighting_shader->bind_texture("in_Texture_Albedo",				temp_buffer1->attachement(0), GL_TEXTURE0);
	tlighting_shader->bind_texture("in_Texture_Emitting",			temp_buffer1->attachement(1), GL_TEXTURE1);
	tlighting_shader->bind_texture("in_Texture_Fresnel",			temp_buffer1->attachement(2), GL_TEXTURE2);
	tlighting_shader->bind_texture("in_Texture_Material_Values",	temp_buffer1->attachement(3), GL_TEXTURE3);
	tlighting_shader->bind_texture("in_Texture_BRDF",				temp_buffer1->attachement(4), GL_TEXTURE4);
	tlighting_shader->bind_texture("in_Texture_AO",					temp_buffer1->attachement(5), GL_TEXTURE5);
	tlighting_shader->bind_texture("in_Texture_Normal",				temp_buffer1->attachement(6), GL_TEXTURE6);
	tlighting_shader->bind_texture("in_Texture_Position",			temp_buffer1->attachement(7), GL_TEXTURE7);
	tlighting_shader->bind_texture("in_Texture_Depth",				temp_buffer1->depth(), GL_TEXTURE8);
	tlighting_shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE9);
	tlighting_shader->bind_texture("Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE10);
	tlighting_shader->bind_texture("in_Back_Color", back_buffer->attachement(0), GL_TEXTURE11);
	tlighting_shader->bind_texture("in_Back_Bright", back_buffer->attachement(1), GL_TEXTURE12);
	Render::display_quad()->draw();
	tlighting_shader->use(false);

	// GENERATE BLOOM FROM out_Brightness
	back_buffer1->attachement(1)->blur(BLOOMPASS, 3.5);

	glEnable(GL_DEPTH_TEST);
	Framebuffer::bind_default();
	presentShader->use();
	presentShader->bind_texture("in_Texture_Color", back_buffer1->attachement(0), GL_TEXTURE0);
	presentShader->bind_texture("in_Texture_Emitting", back_buffer1->attachement(1), GL_TEXTURE1);
	presentShader->bind_texture("in_Texture_Depth", back_buffer1->depth(), GL_TEXTURE2);
	Render::display_quad()->draw();
	presentShader->use(false);
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

void	Render::bind_textures(Shader *)
{
	/*shader->bind_texture("in_Texture_Albedo", temp_buffer1->attachement(0), GL_TEXTURE0); // Albedo;
	shader->bind_texture("in_Texture_Fresnel", temp_buffer1->attachement(1), GL_TEXTURE1); // Fresnel;
	shader->bind_texture("in_Texture_Emitting", temp_buffer1->attachement(2), GL_TEXTURE2); // Emitting;
	shader->bind_texture("in_Texture_Material_Values", temp_buffer1->attachement(3), GL_TEXTURE3); // Material_Values -> Roughness, Metallic
	shader->bind_texture("in_Texture_BRDF", temp_buffer1->attachement(4), GL_TEXTURE4); // irradiance
	shader->bind_texture("in_Texture_Normal", temp_buffer1->attachement(5), GL_TEXTURE5); // Normal;
	shader->bind_texture("in_Texture_Position", temp_buffer1->attachement(6), GL_TEXTURE6); // Position;
	shader->bind_texture("in_Texture_Depth", temp_buffer1->depth(), GL_TEXTURE7);
	shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
	shader->bind_texture("Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE9);*/
}

void		Render::present()
{
	/*MAT4		matrix{{0}};

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
	shader->use(false);*/
}
