/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:42:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/09 10:55:42 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/InternalTools.hpp"
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
	buffer->create_attachement(GL_RED, GL_R8); //AO
	buffer->create_attachement(GL_RGB, GL_RGBA8_SNORM); // Normal;
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

#include <parser/BMP.hpp>

void	present(Framebuffer *back_buffer)
{
	static auto	presentShader = GLSL::parse("present",
		Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/present.frag");

	// GENERATE BLOOM FROM out_Brightness
	back_buffer->attachement(1)->blur(BLOOMPASS, 3.5);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	Framebuffer::bind_default();
	presentShader->use();
	presentShader->bind_texture("in_Texture_Color", back_buffer->attachement(0), GL_TEXTURE0);
	presentShader->bind_texture("in_Texture_Emitting", back_buffer->attachement(1), GL_TEXTURE1);
	presentShader->bind_texture("in_Texture_Depth", back_buffer->depth(), GL_TEXTURE2);
	Render::display_quad()->draw();
	presentShader->use(false);
	Window::swap();
}

void	Render::update()
{
	auto	InvViewMatrix = mat4_inverse(Engine::current_camera()->view);
	auto	InvProjMatrix = mat4_inverse(Engine::current_camera()->projection);
	auto	res = Window::internal_resolution();
	auto	node_index = 0;

	while (auto shader = Engine::shader(node_index))
	{
		shader->use();
		shader->set_uniform("Camera.Position", Engine::current_camera()->position());
		shader->set_uniform("Camera.Matrix.View", Engine::current_camera()->view);
		shader->set_uniform("Camera.Matrix.Projection", Engine::current_camera()->projection);
		shader->set_uniform("Camera.InvMatrix.View", InvViewMatrix);
		shader->set_uniform("Camera.InvMatrix.Projection", InvProjMatrix);
		shader->set_uniform("Resolution", new_vec3(res.x, res.y, res.x / res.y));
		shader->set_uniform("Time", SDL_GetTicks() / 1000.f);
		shader->use(false);
		node_index++;
	}
}

void	Render::scene()
{
	static Texture	*brdf = nullptr;
	if (brdf == nullptr)
	{
		brdf = BMP::parse("brdf", Engine::program_path() + "./res/brdfLUT.bmp");
		brdf->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		brdf->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	static auto	temp_buffer = create_render_buffer("temp_buffer", Window::internal_resolution(), nullptr);
	static auto	temp_buffer1 = create_render_buffer("temp_buffer1", Window::internal_resolution(), nullptr);
	static auto	back_buffer = create_back_buffer("back_buffer", Window::internal_resolution(), nullptr);
	static auto	back_buffer1 = create_back_buffer("back_buffer1", Window::internal_resolution(), nullptr);
	static auto	passthrough_shader = GLSL::parse("passthrough", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/passthrough.frag");
	static auto	lighting_shader = GLSL::parse("lighting", Engine::program_path() + "./res/shaders/lighting.frag", LightingShader);
	static auto	tlighting_shader = GLSL::parse("lighting_transparent", Engine::program_path() + "./res/shaders/lighting_transparent.frag", LightingShader);

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
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	Renderable *node;
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderOpaque);
	}
	
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	for (Shader *shader : post_treatments)
	{
		// APPLY POST-TREATMENT
		temp_buffer->bind();
		shader->use();
		shader->bind_texture("Texture.Albedo",			temp_buffer1->attachement(0), GL_TEXTURE0);
		shader->bind_texture("Texture.Emitting",		temp_buffer1->attachement(1), GL_TEXTURE1);
		shader->bind_texture("Texture.Specular",		temp_buffer1->attachement(2), GL_TEXTURE2);
		shader->bind_texture("Texture.MaterialValues",	temp_buffer1->attachement(3), GL_TEXTURE3);
		shader->bind_texture("Texture.AO",				temp_buffer1->attachement(4), GL_TEXTURE4);
		shader->bind_texture("Texture.Normal",			temp_buffer1->attachement(5), GL_TEXTURE5);
		shader->bind_texture("Texture.Depth",			temp_buffer1->depth(), GL_TEXTURE6);
		shader->bind_texture("Texture.BRDF",				brdf, GL_TEXTURE7);
		shader->bind_texture("Texture.Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
		shader->bind_texture("Texture.Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE9);
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
		passthrough_shader->bind_texture("in_Texture_Depth", temp_buffer->depth(), GL_TEXTURE7);
		Render::display_quad()->draw();
		passthrough_shader->use(false);
	}

	temp_buffer1->attachement(4)->blur(1, 0.75);

	// APPLY LIGHTING SHADER
	// OUTPUT : out_Color, out_Brightness
	back_buffer->bind();
	lighting_shader->use();
	lighting_shader->bind_texture("Texture.Albedo",			temp_buffer1->attachement(0), GL_TEXTURE0);
	lighting_shader->bind_texture("Texture.Emitting",		temp_buffer1->attachement(1), GL_TEXTURE1);
	lighting_shader->bind_texture("Texture.Specular",		temp_buffer1->attachement(2), GL_TEXTURE2);
	lighting_shader->bind_texture("Texture.MaterialValues",	temp_buffer1->attachement(3), GL_TEXTURE3);
	lighting_shader->bind_texture("Texture.AO",				temp_buffer1->attachement(4), GL_TEXTURE4);
	lighting_shader->bind_texture("Texture.Normal",			temp_buffer1->attachement(5), GL_TEXTURE5);
	lighting_shader->bind_texture("Texture.Depth",			temp_buffer1->depth(), GL_TEXTURE6);
	lighting_shader->bind_texture("Texture.BRDF",			brdf, GL_TEXTURE7);
	lighting_shader->bind_texture("Texture.Environment.Diffuse",	Engine::current_environment()->diffuse, GL_TEXTURE8);
	lighting_shader->bind_texture("Texture.Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE9);
	lighting_shader->bind_texture("Texture.Back.Color",				back_buffer1->attachement(0), GL_TEXTURE10);
	lighting_shader->bind_texture("Texture.Back.Bright",			back_buffer1->attachement(1), GL_TEXTURE11);
	lighting_shader->bind_texture("Texture.Back.Depth",				back_buffer1->depth(), GL_TEXTURE12);
	Render::display_quad()->draw();
	lighting_shader->use(false);

	/*
	** ATTEMPT RENDERING TRANSPARENT OBJECTS
	** WRITE DEPTH FOR FUTUR USE
	*/
	temp_buffer1->bind();
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	bool	rendered_stuff = false;
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		if (node->render_depth(RenderTransparent))
			rendered_stuff = true;
	}
	
	if (!rendered_stuff) {
		// NO OBJECTS WERE RENDERED PRESENT IMEDIATLY
		present(back_buffer);
		return ;
	}

	/*
	** REWRITE TRANSPARENT OBJECTS
	** WRITE ONLY CLOSEST OBJECTS
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_EQUAL);
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderTransparent);
	}

	back_buffer->attachement(0)->generate_mipmap();
	back_buffer->attachement(1)->generate_mipmap();

	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	back_buffer1->bind();
	tlighting_shader->use();
	tlighting_shader->bind_texture("Texture.Albedo",			temp_buffer1->attachement(0), GL_TEXTURE0);
	tlighting_shader->bind_texture("Texture.Emitting",			temp_buffer1->attachement(1), GL_TEXTURE1);
	tlighting_shader->bind_texture("Texture.Specular",			temp_buffer1->attachement(2), GL_TEXTURE2);
	tlighting_shader->bind_texture("Texture.MaterialValues",	temp_buffer1->attachement(3), GL_TEXTURE3);
	tlighting_shader->bind_texture("Texture.AO",				temp_buffer1->attachement(4), GL_TEXTURE4);
	tlighting_shader->bind_texture("Texture.Normal",			temp_buffer1->attachement(5), GL_TEXTURE5);
	tlighting_shader->bind_texture("Texture.Depth",				temp_buffer1->depth(), GL_TEXTURE6);
	tlighting_shader->bind_texture("Texture.BRDF",				brdf, GL_TEXTURE7);
	tlighting_shader->bind_texture("Texture.Environment.Diffuse",		Engine::current_environment()->diffuse, GL_TEXTURE8);
	tlighting_shader->bind_texture("Texture.Environment.Irradiance",	Engine::current_environment()->irradiance, GL_TEXTURE9);
	tlighting_shader->bind_texture("Texture.Back.Color",				back_buffer->attachement(0), GL_TEXTURE10);
	tlighting_shader->bind_texture("Texture.Back.Bright",				back_buffer->attachement(1), GL_TEXTURE11);
	tlighting_shader->bind_texture("Texture.Back.Depth",				back_buffer->depth(), GL_TEXTURE12);
	Render::display_quad()->draw();
	tlighting_shader->use(false);

	present(back_buffer1);
}

void	Render::add_post_treatment(Shader *shader)
{
	if (shader != nullptr)
		post_treatments.insert(shader);
}

void	Render::add_post_treatment(const std::string &name, const std::string &path)
{
	auto shader = GLSL::parse(name, path, PostShader);

	if (shader != nullptr)
		post_treatments.insert(shader);
}

void	Render::remove_post_treatment(Shader *shader)
{
	if (shader != nullptr)
		post_treatments.erase(shader);
}
