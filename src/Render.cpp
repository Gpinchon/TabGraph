/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:42:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/15 20:06:34 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/InternalTools.hpp"
#include "Render.hpp"
#include "Engine.hpp"
#include "Light.hpp"
#include "Renderable.hpp"
#include "Camera.hpp"
#include "Cubemap.hpp"
#include "Framebuffer.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include <vector>
#include <algorithm>

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
	buffer->create_attachement(GL_RGB, GL_RGB16_SNORM); // Normal;
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
	back_buffer->attachement(1)->blur(CFG::BloomPass(), 3.5);
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

#include <iostream>
#include "Material.hpp"

void	print_mat4(MAT4 mat)
{
	std::cout << std::endl;
	for (auto x = 0; x < 4; x++) {
		for (auto y = 0; y < 4; y++) {
			std::cout << mat.m[x * 4 + y] << "\t";
		}
		std::cout << std::endl;
	}
}

//TODO Implement
void	render_shadows()
{
	Light	*light;
	static auto	tempCamera = Camera::create("light_camera", 45);
	auto	*camera = Engine::current_camera();	

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	Engine::set_current_camera(tempCamera);
	tempCamera->projection() = mat4_identity();
	for (auto i = 0u; (light = Engine::light(i)) != nullptr;)
	{
		if (!light->cast_shadow()) {
			i++;
			continue ;
		}
		light->render_buffer()->bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		tempCamera->position() = light->position();
		tempCamera->view() = light->mat4_transform();
		for (auto index = 0; Engine::renderable(index) != nullptr; index++) {
			auto	node = Engine::renderable(index);
			node->render_depth(RenderOpaque);
		}
		light->render_buffer()->bind(false);
		i++;
	}
	Engine::set_current_camera(camera);
}

void	Render::fixed_update()
{
	auto	InvViewMatrix = mat4_inverse(Engine::current_camera()->view());
	auto	InvProjMatrix = mat4_inverse(Engine::current_camera()->projection());
	auto	res = Window::internal_resolution();
	auto	index = 0;

	render_shadows();
	while (auto shader = Engine::shader(index))
	{
		shader->use();
		shader->set_uniform("Camera.Position", Engine::current_camera()->position());
		shader->set_uniform("Camera.Matrix.View", Engine::current_camera()->view());
		shader->set_uniform("Camera.Matrix.Projection", Engine::current_camera()->projection());
		shader->set_uniform("Camera.InvMatrix.View", InvViewMatrix);
		shader->set_uniform("Camera.InvMatrix.Projection", InvProjMatrix);
		shader->set_uniform("Resolution", new_vec3(res.x, res.y, res.x / res.y));
		shader->set_uniform("Time", SDL_GetTicks() / 1000.f);
		shader->use(false);
		index++;
	}
}

void	Render::update()
{
	
}

void	light_pass(Framebuffer *&current_backBuffer, Framebuffer *&current_backTexture, Framebuffer *&current_tbuffertex)
{
	static auto	lighting_shader = GLSL::parse("lighting", Engine::program_path() + "./res/shaders/lighting.frag", LightingShader,
		std::string("#define LIGHTNBR ") + std::to_string(CFG::LightsPerPass()) +
		"\n#define PointLight " + std::to_string(Point) +
		"\n#define DirectionnalLight " + std::to_string(Directionnal) +
		"\n");
	lighting_shader->use();
	for (auto i = 0u; Engine::light(i) != nullptr;)
	{
		current_backBuffer->bind();
		auto shaderIndex = 0;
		while (shaderIndex < CFG::LightsPerPass() && Engine::light(i) != nullptr) {
			auto	light = Engine::light(i);
			if (light->cast_shadow() || light->power() == 0 || (!light->color().x && !light->color().y && !light->color().z)) {
				i++;
				continue;
			}
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Position", light->position());
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Color", vec3_scale(light->color(), light->power()));
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Type", light->type());
			i++;
			shaderIndex++;
		}
		if (shaderIndex == 0)
			continue ;
		lighting_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
		lighting_shader->bind_texture("Texture.Emitting",		current_tbuffertex->attachement(1), GL_TEXTURE1);
		lighting_shader->bind_texture("Texture.Specular",		current_tbuffertex->attachement(2), GL_TEXTURE2);
		lighting_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
		lighting_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
		lighting_shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE6);
		lighting_shader->bind_texture("Texture.Back.Color",		current_backTexture->attachement(0), GL_TEXTURE7);
		lighting_shader->bind_texture("Texture.Back.Emitting",	current_backTexture->attachement(1), GL_TEXTURE8);
		lighting_shader->bind_texture("Texture.Back.Depth",		current_backTexture->depth(), GL_TEXTURE9);
		Render::display_quad()->draw();
		std::swap(current_backTexture, current_backBuffer);
	}
	lighting_shader->use(false);
}

void	light_shadow_pass(Framebuffer *&current_backBuffer, Framebuffer *&current_backTexture, Framebuffer *&current_tbuffertex)
{
	static auto	lighting_shader = GLSL::parse("lighting_shadow", Engine::program_path() + "./res/shaders/lighting.frag", LightingShader,
		std::string("#define LIGHTNBR ") + std::to_string(CFG::ShadowsPerPass()) +
		"\n#define PointLight " + std::to_string(Point) +
		"\n#define DirectionnalLight " + std::to_string(Directionnal) +
		"\n#define SHADOW" +
		"\n");
	lighting_shader->use();
	for (auto i = 0u; Engine::light(i) != nullptr;)
	{
		current_backBuffer->bind();
		auto shaderIndex = 0;
		while (shaderIndex < CFG::ShadowsPerPass() && Engine::light(i) != nullptr) {
			auto	light = Engine::light(i);
			if (!light->cast_shadow() || light->power() == 0 || (!light->color().x && !light->color().y && !light->color().z)) {
				i++;
				continue;
			}
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Position", light->position());
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Color", vec3_scale(light->color(), light->power()));
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Type", light->type());
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Projection", light->mat4_transform());
			lighting_shader->bind_texture("Light[" + std::to_string(shaderIndex) + "].Shadow", light->render_buffer()->depth(), GL_TEXTURE10 + shaderIndex);
			i++;
			shaderIndex++;
		}
		if (shaderIndex == 0)
			continue ;
		lighting_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
		lighting_shader->bind_texture("Texture.Emitting",		current_tbuffertex->attachement(1), GL_TEXTURE1);
		lighting_shader->bind_texture("Texture.Specular",		current_tbuffertex->attachement(2), GL_TEXTURE2);
		lighting_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
		lighting_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
		lighting_shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE6);
		lighting_shader->bind_texture("Texture.Back.Color",		current_backTexture->attachement(0), GL_TEXTURE7);
		lighting_shader->bind_texture("Texture.Back.Emitting",	current_backTexture->attachement(1), GL_TEXTURE8);
		lighting_shader->bind_texture("Texture.Back.Depth",		current_backTexture->depth(), GL_TEXTURE9);
		Render::display_quad()->draw();
		std::swap(current_backTexture, current_backBuffer);
	}
	lighting_shader->use(false);
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
	static auto	back_buffer2 = create_back_buffer("back_buffer1", Window::internal_resolution(), nullptr);
	static auto	elighting_shader = GLSL::parse("lighting_env", Engine::program_path() + "./res/shaders/lighting_env.frag", LightingShader);
	static auto	telighting_shader = GLSL::parse("lighting_env_transparent", Engine::program_path() + "./res/shaders/lighting_env.frag", LightingShader, "#define TRANSPARENT\n");
	static auto	refraction_shader = GLSL::parse("refraction", Engine::program_path() + "./res/shaders/refraction.frag", LightingShader);

	temp_buffer->resize(Window::internal_resolution());
	temp_buffer1->resize(Window::internal_resolution());
	back_buffer->resize(Window::internal_resolution());
	back_buffer1->resize(Window::internal_resolution());
	back_buffer2->resize(Window::internal_resolution());

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

	auto	current_tbuffer = temp_buffer;
	auto	current_tbuffertex = temp_buffer1;

	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	for (Shader *shader : post_treatments)
	{
		// APPLY POST-TREATMENT
		current_tbuffer->bind();
		shader->use();
		shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
		shader->bind_texture("Texture.Emitting",		current_tbuffertex->attachement(1), GL_TEXTURE1);
		shader->bind_texture("Texture.Specular",		current_tbuffertex->attachement(2), GL_TEXTURE2);
		shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
		shader->bind_texture("Texture.AO",				current_tbuffertex->attachement(4), GL_TEXTURE4);
		shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
		shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE6);
		shader->bind_texture("Texture.BRDF",				brdf, GL_TEXTURE7);
		shader->bind_texture("Texture.Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
		shader->bind_texture("Texture.Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE9);
		Render::display_quad()->draw();
		shader->use(false);

		std::swap(current_tbuffer, current_tbuffertex);
	}

	current_tbuffertex->attachement(4)->blur(1, 0.75);

	back_buffer1->bind();
	glClear(GL_COLOR_BUFFER_BIT);

	//static auto	Lights = create_random_lights(200);

	auto	current_backBuffer = back_buffer;
	auto	current_backTexture = back_buffer1;
	light_pass(current_backBuffer, current_backTexture, current_tbuffertex);
	light_shadow_pass(current_backBuffer, current_backTexture, current_tbuffertex);

	// APPLY LIGHTING SHADER
	// OUTPUT : out_Color, out_Brightness
	current_backBuffer->bind();
	elighting_shader->use();

	elighting_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
	elighting_shader->bind_texture("Texture.Emitting",			current_tbuffertex->attachement(1), GL_TEXTURE1);
	elighting_shader->bind_texture("Texture.Specular",			current_tbuffertex->attachement(2), GL_TEXTURE2);
	elighting_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
	elighting_shader->bind_texture("Texture.AO",				current_tbuffertex->attachement(4), GL_TEXTURE4);
	elighting_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
	elighting_shader->bind_texture("Texture.Depth",				current_tbuffertex->depth(), GL_TEXTURE6);
	elighting_shader->bind_texture("Texture.BRDF",				brdf, GL_TEXTURE7);
	elighting_shader->bind_texture("Texture.Environment.Diffuse",		Engine::current_environment()->diffuse, GL_TEXTURE8);
	elighting_shader->bind_texture("Texture.Environment.Irradiance",	Engine::current_environment()->irradiance, GL_TEXTURE9);
	elighting_shader->bind_texture("Texture.Back.Color",				current_backTexture->attachement(0), GL_TEXTURE10);
	elighting_shader->bind_texture("Texture.Back.Emitting",				current_backTexture->attachement(1), GL_TEXTURE11);
	elighting_shader->bind_texture("Texture.Back.Depth",				current_backTexture->depth(), GL_TEXTURE12);
	Render::display_quad()->draw();
	elighting_shader->use(false);

	std::swap(current_backTexture, current_backBuffer);

	//present(current_backTexture);

	// ATTEMPT RENDERING TRANSPARENT OBJECTS
	// WRITE DEPTH FOR FUTUR USE
	current_tbuffer->bind();
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
		present(current_backTexture);
		return ;
	}

	// REWRITE TRANSPARENT OBJECTS
	// WRITE ONLY CLOSEST OBJECTS
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_EQUAL);
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderTransparent);
	}

	auto	opaqueBackBuffer = current_backTexture;
	opaqueBackBuffer->attachement(0)->generate_mipmap();
	opaqueBackBuffer->attachement(1)->generate_mipmap();

	current_backTexture = back_buffer2;
	back_buffer2->bind();
	glClear(GL_COLOR_BUFFER_BIT);

	std::swap(current_tbuffer, current_tbuffertex);

	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	light_pass(current_backBuffer, current_backTexture, current_tbuffertex);

	current_backBuffer->bind();
	telighting_shader->use();
	telighting_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
	telighting_shader->bind_texture("Texture.Emitting",			current_tbuffertex->attachement(1), GL_TEXTURE1);
	telighting_shader->bind_texture("Texture.Specular",			current_tbuffertex->attachement(2), GL_TEXTURE2);
	telighting_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
	telighting_shader->bind_texture("Texture.AO",				current_tbuffertex->attachement(4), GL_TEXTURE4);
	telighting_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
	telighting_shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE6);
	telighting_shader->bind_texture("Texture.BRDF",				brdf, GL_TEXTURE7);
	telighting_shader->bind_texture("Texture.Environment.Diffuse",		Engine::current_environment()->diffuse, GL_TEXTURE8);
	telighting_shader->bind_texture("Texture.Environment.Irradiance",	Engine::current_environment()->irradiance, GL_TEXTURE9);
	telighting_shader->bind_texture("Texture.Back.Color",				current_backTexture->attachement(0), GL_TEXTURE10);
	telighting_shader->bind_texture("Texture.Back.Emitting",			current_backTexture->attachement(1), GL_TEXTURE11);
	telighting_shader->bind_texture("Texture.Back.Depth",				current_backTexture->depth(), GL_TEXTURE12);

	Render::display_quad()->draw();
	telighting_shader->use(false);

	std::swap(current_backTexture, current_backBuffer);

	refraction_shader->use();
	current_backBuffer->bind();
	refraction_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
	refraction_shader->bind_texture("Texture.Specular",			current_tbuffertex->attachement(2), GL_TEXTURE1);
	refraction_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE2);
	refraction_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE3);
	refraction_shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE4);
	refraction_shader->bind_texture("Texture.Back.Color",		current_backTexture->attachement(0), GL_TEXTURE5);
	refraction_shader->bind_texture("Texture.Back.Emitting",	current_backTexture->attachement(1), GL_TEXTURE6);
	refraction_shader->bind_texture("opaqueBackColor",			opaqueBackBuffer->attachement(0), GL_TEXTURE7);
	refraction_shader->bind_texture("opaqueBackEmitting",		opaqueBackBuffer->attachement(1), GL_TEXTURE8);
	Render::display_quad()->draw();
	refraction_shader->use(false);

	present(current_backBuffer);
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
