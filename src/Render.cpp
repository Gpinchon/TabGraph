/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:42:59 by gpinchon          #+#    #+#             */
/*   Updated: 2019/03/03 19:59:01 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Render.hpp"
#include "Camera.hpp"
#include "Config.hpp"
#include "Cubemap.hpp"
#include "Engine.hpp"
#include "Environment.hpp"
#include "Framebuffer.hpp"
#include "Light.hpp"
#include "Renderable.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "Window.hpp"
#include "parser/BMP.hpp"
#include "parser/InternalTools.hpp"
#include <algorithm>
#include <vector>

static auto passthrough_vertex_code =
#include "passthrough.vert"
    ;

static auto present_fragment_code =
#include "present.frag"
    ;

static auto emptyShaderCode =
#include "empty.glsl"
    ;

/*
** quad is a singleton
*/

const std::shared_ptr<VertexArray> Render::display_quad()
{
    static std::shared_ptr<VertexArray> vao;
    if (vao != nullptr) {
        return (vao);
    }
    vao = VertexArray::create(4, GL_TRIANGLE_STRIP);
    std::vector<float> quad(8);
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

std::shared_ptr<Framebuffer> create_render_buffer(const std::string& name, const VEC2& size)
{
    auto buffer = Framebuffer::create(name, size, 0, 1);
    buffer->create_attachement(GL_RGBA, GL_RGBA8); // Albedo;
    buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emitting;
    buffer->create_attachement(GL_RGB, GL_RGB8); // Fresnel;
    buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Material_Values -> Roughness, Metallic, Ior
    buffer->create_attachement(GL_RED, GL_R8); //AO
    buffer->create_attachement(GL_RGB, GL_RGB16_SNORM); // Normal;
    buffer->setup_attachements();
    return (buffer);
}

std::shared_ptr<Framebuffer> create_back_buffer(const std::string& name, const VEC2& size)
{
    auto buffer = Framebuffer::create(name, size, 0, 1);
    buffer->create_attachement(GL_RGBA, GL_RGBA8); // Color;
    buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emitting;
    buffer->create_attachement(GL_RGB, GL_RGB16_SNORM); //Normal
    buffer->setup_attachements();
    return (buffer);
}

void present(std::shared_ptr<Framebuffer> back_buffer)
{
    static auto presentShader = GLSL::compile("present",
        passthrough_vertex_code, present_fragment_code);

    // GENERATE BLOOM FROM out_Brightness
    back_buffer->attachement(1)->blur(Config::BloomPass(), 3.5);
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

#include "Material.hpp"
#include <iostream>

void print_mat4(MAT4 mat)
{
    std::cout << std::endl;
    for (auto x = 0; x < 4; x++) {
        for (auto y = 0; y < 4; y++) {
            std::cout << mat.m[x * 4 + y] << "\t";
        }
        std::cout << std::endl;
    }
}

std::vector<std::shared_ptr<Light>> normalLights;
std::vector<std::shared_ptr<Light>> shadowLights;

//TODO Cleanup
void render_shadows()
{
    static auto tempCamera = Camera::create("light_camera", 45);
    auto camera = Camera::current();

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Camera::set_current(tempCamera);
    tempCamera->projection() = mat4_identity();
    for (auto light : shadowLights) {
        light->render_buffer()->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        tempCamera->position() = light->position();
        tempCamera->view() = light->transform();
        for (auto index = 0; Renderable::get(index) != nullptr; index++) {
            auto node = Renderable::get(index);
            node->render_depth(RenderOpaque);
        }
        light->render_buffer()->bind(false);
    }
    Camera::set_current(camera);
}

void Render::fixed_update()
{
    auto InvViewMatrix = mat4_inverse(Camera::current()->view());
    auto InvProjMatrix = mat4_inverse(Camera::current()->projection());
    auto res = Window::internal_resolution();
    auto index = 0;

    shadowLights.reserve(1000);
    normalLights.reserve(1000);
    shadowLights.clear();
    normalLights.clear();
    while (auto light = Light::get(index)) {
        if (light->power() == 0 || (!light->color().x && !light->color().y && !light->color().z))
            continue;
        if (light->cast_shadow())
            shadowLights.push_back(light);
        else
            normalLights.push_back(light);
        index++;
    }
    render_shadows();
    index = 0;
    while (auto shader = Shader::get(index)) {
        shader->use();
        shader->set_uniform("Camera.Position", Camera::current()->position());
        shader->set_uniform("Camera.Matrix.View", Camera::current()->view());
        shader->set_uniform("Camera.Matrix.Projection", Camera::current()->projection());
        shader->set_uniform("Camera.InvMatrix.View", InvViewMatrix);
        shader->set_uniform("Camera.InvMatrix.Projection", InvProjMatrix);
        shader->set_uniform("Resolution", new_vec3(res.x, res.y, res.x / res.y));
        shader->set_uniform("Time", SDL_GetTicks() / 1000.f);
        shader->use(false);
        index++;
    }
}

void Render::update()
{
}

void light_pass(std::shared_ptr<Framebuffer>& current_backBuffer, std::shared_ptr<Framebuffer>& current_backTexture, std::shared_ptr<Framebuffer>& current_tbuffertex)
{
    if (Config::LightsPerPass() == 0)
        return;
    static auto lightingFragmentCode =
#include "lighting.frag"
        ;
    static auto lighting_shader = GLSL::compile("lighting", lightingFragmentCode, LightingShader,
        std::string("\n#define LIGHTNBR				") + std::to_string(Config::LightsPerPass()) + std::string("\n#define PointLight			") + std::to_string(Point) + std::string("\n#define DirectionnalLight	") + std::to_string(Directionnal) + std::string("\n"));
    static auto slighting_shader = GLSL::compile("shadow_lighting", lightingFragmentCode, LightingShader,
        (Config::ShadowsPerPass() > 0 ? std::string("\n#define SHADOW") : std::string("\n")) + std::string("\n#define SHADOWNBR			") + std::to_string(Config::ShadowsPerPass()) + std::string("\n#define LIGHTNBR				") + std::to_string(Config::LightsPerPass()) + std::string("\n#define PointLight			") + std::to_string(Point) + std::string("\n#define DirectionnalLight	") + std::to_string(Directionnal) + std::string("\n"));
    auto actualShadowNbr = std::max(uint16_t(1), Config::ShadowsPerPass());
    auto shader = lighting_shader;
    for (auto i = 0u, j = 0u; i < normalLights.size() || j < shadowLights.size();) {
        if (normalLights.size() - i < Config::LightsPerPass() && shadowLights.size() - j != 0)
            shader = slighting_shader;
        else
            shader = lighting_shader;
        current_backBuffer->bind();
        shader->use();
        auto lightIndex = 0u;
        while (lightIndex < Config::LightsPerPass() && i < normalLights.size()) {
            auto light = normalLights.at(i);
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Position", light->position());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Color", vec3_scale(light->color(), light->power()));
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Type", light->type());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].ShadowIndex", -1);
            i++;
            lightIndex++;
        }
        auto shadowIndex = 0u;
        while (lightIndex < Config::LightsPerPass() && shadowIndex < actualShadowNbr && j < shadowLights.size()) {
            auto light = shadowLights.at(j);
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Position", light->position());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Color", vec3_scale(light->color(), light->power()));
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Type", light->type());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].ShadowIndex", int(shadowIndex));
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Projection", light->transform());
            shader->bind_texture("Shadow[" + std::to_string(shadowIndex) + "]", light->render_buffer()->depth(), GL_TEXTURE9 + shadowIndex);
            j++;
            lightIndex++;
            shadowIndex++;
        }
        if (lightIndex == 0)
            continue;
        shader->bind_texture("Texture.Albedo", current_tbuffertex->attachement(0), GL_TEXTURE0);
        shader->bind_texture("Texture.Emitting", current_tbuffertex->attachement(1), GL_TEXTURE1);
        shader->bind_texture("Texture.Specular", current_tbuffertex->attachement(2), GL_TEXTURE2);
        shader->bind_texture("Texture.MaterialValues", current_tbuffertex->attachement(3), GL_TEXTURE3);
        shader->bind_texture("Texture.Normal", current_tbuffertex->attachement(5), GL_TEXTURE5);
        shader->bind_texture("Texture.Depth", current_tbuffertex->depth(), GL_TEXTURE6);
        shader->bind_texture("Texture.Back.Color", current_backTexture->attachement(0), GL_TEXTURE7);
        shader->bind_texture("Texture.Back.Emitting", current_backTexture->attachement(1), GL_TEXTURE8);
        shader->bind_texture("Texture.Back.Normal", current_backTexture->attachement(2), GL_TEXTURE8);
        Render::display_quad()->draw();
        std::swap(current_backTexture, current_backBuffer);
    }
    shader->use(false);
}

#include <fstream>

void Render::scene()
{
    static std::shared_ptr<Texture> brdf;
    if (brdf == nullptr) {
        brdf = Texture::parse("brdf", Engine::resource_path() + "/brdfLUT.bmp");
        //brdf = BMP::parse("brdf", Engine::resource_path() + "/brdfLUT.bmp");
        /*static std::ofstream myfile;
		myfile.open ("brdfLUT");
		for (auto x = brdf->size().x; x > 0; x--) {
			for (auto y = 0; y < brdf->size().y; y++) {
				VEC2	uv;
				uv.x = x / brdf->size().x;
				uv.y = y / brdf->size().y;
				auto pixel = brdf->sample(uv);
				myfile << int(pixel.z * 255.f) << ", " << int(pixel.y * 255.f) << ", ";
			}
			myfile << std::endl;
		}*/
        brdf->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        brdf->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    static auto temp_buffer = create_render_buffer("temp_buffer", Window::internal_resolution());
    static auto temp_buffer1 = create_render_buffer("temp_buffer1", Window::internal_resolution());
    static auto back_buffer = create_back_buffer("back_buffer", Window::internal_resolution());
    static auto back_buffer1 = create_back_buffer("back_buffer1", Window::internal_resolution());
    static auto back_buffer2 = create_back_buffer("back_buffer2", Window::internal_resolution());
    static auto final_back_buffer = create_back_buffer("final_back_buffer", Window::internal_resolution());
    static auto lightingEnvFragmentCode =
#include "lighting_env.frag"
        ;
    static auto refractionFragmentCode =
#include "refraction.frag"
        ;
    static auto elighting_shader = GLSL::compile("lighting_env", lightingEnvFragmentCode, LightingShader,
        std::string("\n#define REFLEXION_STEPS		") + std::to_string(Config::ReflexionSteps()) + std::string("\n#define REFLEXION_SAMPLES	") + std::to_string(Config::ReflexionSamples()) + std::string("\n#define SCREEN_BORDER_FACTOR	") + std::to_string(Config::ReflexionBorderFactor()) + std::string("\n"));
    static auto telighting_shader = GLSL::compile("lighting_env_transparent", lightingEnvFragmentCode, LightingShader,
        std::string("\n#define TRANSPARENT") + std::string("\n#define REFLEXION_STEPS		") + std::to_string(Config::ReflexionSteps()) + std::string("\n#define REFLEXION_SAMPLES	") + std::to_string(Config::ReflexionSamples()) + std::string("\n#define SCREEN_BORDER_FACTOR	") + std::to_string(Config::ReflexionBorderFactor()) + std::string("\n"));
    static auto refraction_shader = GLSL::compile("refraction", refractionFragmentCode, LightingShader);
    static auto passthrough_shader = GLSL::compile("passthrough", emptyShaderCode, LightingShader);

    temp_buffer->resize(Window::internal_resolution());
    temp_buffer1->resize(Window::internal_resolution());
    back_buffer->resize(Window::internal_resolution());
    back_buffer1->resize(Window::internal_resolution());
    back_buffer2->resize(Window::internal_resolution());
    final_back_buffer->resize(Window::internal_resolution());

    auto current_tbuffer = temp_buffer;
    auto current_tbuffertex = temp_buffer1;

    /*
	** TODO :
	** 	- CLEANUP CODE
	*/
    glClearColor(0, 0, 0, 0);
    current_tbuffer->bind();
    glClear(Window::clear_mask());
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    std::shared_ptr<Renderable> node;
    for (auto index = 0; (node = Renderable::get(index)) != nullptr; index++) {
        node->render(RenderOpaque);
    }
    std::swap(current_tbuffer, current_tbuffertex);

    glDepthFunc(GL_ALWAYS);
    glDisable(GL_CULL_FACE);
    if (post_treatments().size() == 0u) {
        current_tbuffer->bind();
        passthrough_shader->use();
        passthrough_shader->bind_texture("Texture.Albedo", current_tbuffertex->attachement(0), GL_TEXTURE0);
        passthrough_shader->bind_texture("Texture.Emitting", current_tbuffertex->attachement(1), GL_TEXTURE1);
        passthrough_shader->bind_texture("Texture.Specular", current_tbuffertex->attachement(2), GL_TEXTURE2);
        passthrough_shader->bind_texture("Texture.MaterialValues", current_tbuffertex->attachement(3), GL_TEXTURE3);
        passthrough_shader->bind_texture("Texture.AO", current_tbuffertex->attachement(4), GL_TEXTURE4);
        passthrough_shader->bind_texture("Texture.Normal", current_tbuffertex->attachement(5), GL_TEXTURE5);
        passthrough_shader->bind_texture("Texture.Depth", current_tbuffertex->depth(), GL_TEXTURE6);
        Render::display_quad()->draw();
        passthrough_shader->use(false);
    }
    for (auto shader : post_treatments()) {
        // APPLY POST-TREATMENT
        auto shaderPtr = shader.lock();
        current_tbuffer->bind();
        shaderPtr->use();
        shaderPtr->bind_texture("Texture.Albedo", current_tbuffertex->attachement(0), GL_TEXTURE0);
        shaderPtr->bind_texture("Texture.Emitting", current_tbuffertex->attachement(1), GL_TEXTURE1);
        shaderPtr->bind_texture("Texture.Specular", current_tbuffertex->attachement(2), GL_TEXTURE2);
        shaderPtr->bind_texture("Texture.MaterialValues", current_tbuffertex->attachement(3), GL_TEXTURE3);
        shaderPtr->bind_texture("Texture.AO", current_tbuffertex->attachement(4), GL_TEXTURE4);
        shaderPtr->bind_texture("Texture.Normal", current_tbuffertex->attachement(5), GL_TEXTURE5);
        shaderPtr->bind_texture("Texture.Depth", current_tbuffertex->depth(), GL_TEXTURE6);
        shaderPtr->bind_texture("Texture.BRDF", brdf, GL_TEXTURE7);
        if (Environment::current() != nullptr) {
            shaderPtr->bind_texture("Texture.Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE8);
            shaderPtr->bind_texture("Texture.Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE9);
        }
        Render::display_quad()->draw();
        shaderPtr->use(false);

        std::swap(current_tbuffer, current_tbuffertex);
    }
    current_tbuffertex->attachement(4)->blur(1, 0.75);

    auto current_backBuffer = back_buffer;
    auto current_backTexture = back_buffer1;

    current_backTexture->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    light_pass(current_backBuffer, current_backTexture, current_tbuffertex);

    final_back_buffer->attachement(0)->generate_mipmap();
    final_back_buffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    final_back_buffer->depth()->generate_mipmap();
    final_back_buffer->depth()->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // APPLY LIGHTING SHADER
    // OUTPUT : out_Color, out_Brightness
    current_backBuffer->bind();
    elighting_shader->use();
    auto i = 1;
    elighting_shader->bind_texture("Texture.Albedo", current_tbuffertex->attachement(0), GL_TEXTURE0);
    elighting_shader->bind_texture("Texture.Emitting", current_tbuffertex->attachement(1), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.Specular", current_tbuffertex->attachement(2), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.MaterialValues", current_tbuffertex->attachement(3), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.AO", current_tbuffertex->attachement(4), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.Normal", current_tbuffertex->attachement(5), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.Depth", current_tbuffertex->depth(), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.BRDF", brdf, GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.Back.Color", current_backTexture->attachement(0), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.Back.Emitting", current_backTexture->attachement(1), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("Texture.Back.Normal", current_backTexture->attachement(2), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("LastColor", final_back_buffer->attachement(0), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("LastNormal", final_back_buffer->attachement(2), GL_TEXTURE0 + (++i));
    elighting_shader->bind_texture("LastDepth", final_back_buffer->depth(), GL_TEXTURE0 + (++i));
    if (Environment::current() != nullptr) {
        elighting_shader->bind_texture("Texture.Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE0 + (++i));
        elighting_shader->bind_texture("Texture.Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE0 + (++i));
    }
    Render::display_quad()->draw();
    elighting_shader->use(false);
    std::swap(current_backTexture, current_backBuffer);

    current_tbuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    bool rendered_stuff = false;
    for (auto index = 0; (node = Renderable::get(index)) != nullptr; index++) {
        if (node->render(RenderTransparent))
            rendered_stuff = true;
    }
    std::swap(current_tbuffer, current_tbuffertex);

    if (!rendered_stuff) {
        // NO OBJECTS WERE RENDERED PRESENT IMEDIATLY
        //present(current_backTexture);
        //return ;
    }

    auto opaqueBackBuffer = current_backTexture;
    opaqueBackBuffer->attachement(0)->generate_mipmap();
    opaqueBackBuffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    opaqueBackBuffer->depth()->generate_mipmap();
    opaqueBackBuffer->depth()->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    current_backTexture = back_buffer2;
    back_buffer2->bind();
    glClear(GL_COLOR_BUFFER_BIT);

    glDepthFunc(GL_ALWAYS);
    glDisable(GL_CULL_FACE);
    light_pass(current_backBuffer, current_backTexture, current_tbuffertex);

    current_backBuffer->bind();
    telighting_shader->use();
    i = 1;
    telighting_shader->bind_texture("Texture.Albedo", current_tbuffertex->attachement(0), GL_TEXTURE0);
    telighting_shader->bind_texture("Texture.Emitting", current_tbuffertex->attachement(1), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.Specular", current_tbuffertex->attachement(2), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.MaterialValues", current_tbuffertex->attachement(3), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.AO", current_tbuffertex->attachement(4), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.Normal", current_tbuffertex->attachement(5), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.Depth", current_tbuffertex->depth(), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.BRDF", brdf, GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.Back.Color", current_backTexture->attachement(0), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.Back.Emitting", current_backTexture->attachement(1), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("Texture.Back.Normal", current_backTexture->attachement(2), GL_TEXTURE0 + (++i));
    //telighting_shader->bind_texture("opaqueBackColor", opaqueBackBuffer->attachement(0), GL_TEXTURE0 + (++i));
    //telighting_shader->bind_texture("opaqueBackEmitting", opaqueBackBuffer->attachement(1), GL_TEXTURE0 + (++i));
    //telighting_shader->bind_texture("opaqueBackNormal", opaqueBackBuffer->attachement(2), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("LastColor", final_back_buffer->attachement(0), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("LastNormal", final_back_buffer->attachement(2), GL_TEXTURE0 + (++i));
    telighting_shader->bind_texture("LastDepth", final_back_buffer->depth(), GL_TEXTURE0 + (++i));
    if (Environment::current() != nullptr) {
        telighting_shader->bind_texture("Texture.Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE0 + (++i));
        telighting_shader->bind_texture("Texture.Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE0 + (++i));
    }
    Render::display_quad()->draw();
    telighting_shader->use(false);
    std::swap(current_backTexture, current_backBuffer);

    refraction_shader->use();
    final_back_buffer->bind();
    i = 1;
    refraction_shader->bind_texture("Texture.Albedo", current_tbuffertex->attachement(0), GL_TEXTURE0);
    refraction_shader->bind_texture("Texture.Specular", current_tbuffertex->attachement(2), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("Texture.MaterialValues", current_tbuffertex->attachement(3), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("Texture.Normal", current_tbuffertex->attachement(5), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("Texture.Depth", current_tbuffertex->depth(), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("Texture.Back.Color", current_backTexture->attachement(0), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("Texture.Back.Emitting", current_backTexture->attachement(1), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("Texture.Back.Normal", current_backTexture->attachement(2), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("opaqueBackColor", opaqueBackBuffer->attachement(0), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("opaqueBackEmitting", opaqueBackBuffer->attachement(1), GL_TEXTURE0 + (++i));
    refraction_shader->bind_texture("opaqueBackNormal", opaqueBackBuffer->attachement(2), GL_TEXTURE0 + (++i));
    Render::display_quad()->draw();
    refraction_shader->use(false);

    final_back_buffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    final_back_buffer->attachement(1)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    final_back_buffer->attachement(2)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    opaqueBackBuffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    opaqueBackBuffer->attachement(1)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    present(final_back_buffer);
}

std::vector<std::weak_ptr<Shader>>& Render::post_treatments()
{
    static std::vector<std::weak_ptr<Shader>> ptVector;
    return (ptVector);
}

void Render::add_post_treatment(std::shared_ptr<Shader> shader)
{
    if (shader != nullptr)
        post_treatments().push_back(shader);
}

void Render::add_post_treatment(const std::string& name, const std::string& path)
{
    auto shader = GLSL::parse(name, path, PostShader);

    if (shader != nullptr)
        post_treatments().push_back(shader);
}

void Render::remove_post_treatment(std::shared_ptr<Shader> shader)
{
    if (shader != nullptr) {
        post_treatments().erase(std::remove_if(
                                    post_treatments().begin(),
                                    post_treatments().end(),
                                    [shader](std::weak_ptr<Shader> p) { return !(p.owner_before(shader) || shader.owner_before(p)); }),
            post_treatments().end()); //post_treatments.erase(shader);
    }
}
