/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-10-29 17:27:18
*/

#include "Render.hpp"
#include "Camera.hpp" // for Camera
#include "Config.hpp" // for Config
#include "Cubemap.hpp"
#include "Engine.hpp" // for UpdateMutex, SwapInterval
#include "Environment.hpp" // for Environment
#include "Framebuffer.hpp" // for Framebuffer
#include "Light.hpp" // for Light, Directionnal, Point
#include "Renderable.hpp" // for Renderable, RenderOpaque, RenderTransparent
#include "Shader.hpp" // for Shader
#include "Texture.hpp" // for Texture
#include "VertexArray.hpp" // for VertexArray
#include "Window.hpp" // for Window
#include "brdfLUT.hpp" // for brdfLUT
#include "glm/glm.hpp" // for glm::inverse, vec2_scale, vec3_scale
#include "parser/GLSL.hpp" // for GLSL, LightingShader, PostShader
#include <GL/glew.h> // for GL_TEXTURE0, glDepthFunc, glClear, glDis...
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <SDL2/SDL_video.h> // for SDL_GL_MakeCurrent, SDL_GL_SetSwapInterval
#include <algorithm> // for max, remove_if
#include <atomic> // for atomic
#include <iostream> // for char_traits, endl, cout, operator<<, ost...
#include <mutex> // for mutex
#include <stdint.h> // for uint64_t, uint16_t
#include <string> // for operator+, to_string, string
#include <thread> // for thread
#include <vector> // for vector<>::iterator, vector

namespace Render
{
class Private
{
    //class Render::Private {
public:
    static void Update();
    static void FixedUpdate();
    static void Scene();
    static void AddPostTreatment(std::shared_ptr<Shader>);
    static void AddPostTreatment(const std::string &name, const std::string &path);
    static void RemovePostTreatment(std::shared_ptr<Shader>);
    static void StartRenderingThread();
    static void StopRenderingThread();
    static void RequestRedraw();
    static double DeltaTime();
    static bool NeedsUpdate();
    static uint64_t FrameNbr(void);
    static const std::shared_ptr<VertexArray> DisplayQuad();
    static std::vector<std::weak_ptr<Shader>> &PostTreatments();
    static void SetInternalQuality(float);
    static float InternalQuality();

private:
    static void _thread();
    static Render::Private &_instance();
    std::atomic<double> _deltaTime{0};
    std::atomic<float> _internalQuality{1};
    std::atomic<bool> _needsUpdate{true};
    std::atomic<bool> _loop{true};
    uint64_t _frame_nbr{0};
    std::thread _rendering_thread;
};
} // namespace Render

static auto passthrough_vertex_code =
#include "passthrough.vert"
    ;

static auto passthrough_fragment_code =
#include "passthrough.frag"
    ;

static auto present_fragment_code =
#include "present.frag"
    ;

static auto emptyShaderCode =
#include "empty.glsl"
    ;

static auto lightingFragmentCode =
#include "lighting.frag"
    ;
static auto lightingEnvFragmentCode =
#include "lighting_env.frag"
    ;
static auto refractionFragmentCode =
#include "refraction.frag"
    ;
static auto SSRShaderCode =
#include "SSR.frag"
    ;
static auto SSRMergeShaderCode =
#include "SSRMerge.frag"
    ;
static auto SSRBlurShaderCode =
#include "SSRBlur.frag"
    ;

/*
** quad is a singleton
*/

const std::shared_ptr<VertexArray> Render::Private::DisplayQuad()
{
    static std::shared_ptr<VertexArray> vao;
    if (vao != nullptr)
    {
        return (vao);
    }
    vao = VertexArray::Create(4, GL_TRIANGLE_STRIP);
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

void DrawNodes(std::shared_ptr<Node> rootNode, RenderMod mode) {
    if (auto renderable = std::dynamic_pointer_cast<Renderable>(rootNode))
        renderable->Draw(mode);
    std::shared_ptr<Node> child;
    for (const auto &child : rootNode->Children())
        DrawNodes(child, mode);
}

void DrawNodes(RenderMod mode) {
    std::shared_ptr<Node> node(nullptr);
    for (auto nodeIndex = 0; (node = Node::Get(nodeIndex)) != nullptr; nodeIndex++)
        DrawNodes(node, mode);
}

void DrawNodesDepth(std::shared_ptr<Node> rootNode, RenderMod mode) {
    if (auto renderable = std::dynamic_pointer_cast<Renderable>(rootNode))
        renderable->DrawDepth(mode);
    std::shared_ptr<Node> child;
    for (const auto &child : rootNode->Children())
        DrawNodesDepth(child, mode);
}

void DrawNodesDepth(RenderMod mode) {
    std::shared_ptr<Node> node(nullptr);
    for (auto nodeIndex = 0; (node = Node::Get(nodeIndex)) != nullptr; nodeIndex++)
        DrawNodesDepth(node, mode);
}

std::shared_ptr<Framebuffer> Create_render_buffer(const std::string &name, const glm::ivec2 &size)
{
    auto buffer = Framebuffer::Create(name, size, 0, 1);
    buffer->Create_attachement(GL_RGBA, GL_RGBA8); // Albedo;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emitting;
    buffer->Create_attachement(GL_RGB, GL_RGB8); // Fresnel;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Material_Values -> Roughness, Metallic, Ior
    buffer->Create_attachement(GL_RED, GL_R8); //AO
    buffer->Create_attachement(GL_RGB, GL_RGB16_SNORM); // Normal;
    buffer->setup_attachements();
    return (buffer);
}

std::shared_ptr<Framebuffer> Create_back_buffer(const std::string &name, const glm::ivec2 &size)
{
    auto buffer = Framebuffer::Create(name, size, 0, 1);
    buffer->Create_attachement(GL_RGBA, GL_RGBA8); // Color;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emitting;
    buffer->Create_attachement(GL_RGB, GL_RGB16_SNORM); //Normal
    buffer->setup_attachements();
    return (buffer);
}

void present(std::shared_ptr<Framebuffer> back_buffer)
{
    static auto presentShader = GLSL::compile("present",
                                              passthrough_vertex_code, present_fragment_code);

    glDepthFunc(GL_ALWAYS);
    glDisable(GL_CULL_FACE);
    Framebuffer::bind_default();
    presentShader->use();
    presentShader->bind_texture("in_Texture_Color", back_buffer->attachement(0), GL_TEXTURE0);
    presentShader->bind_texture("in_Texture_Emitting", back_buffer->attachement(1), GL_TEXTURE1);
    presentShader->bind_texture("in_Texture_Depth", back_buffer->depth(), GL_TEXTURE2);
    Render::Private::DisplayQuad()->draw();
    presentShader->use(false);
    Window::swap();
}

std::vector<std::shared_ptr<Light>> normalLights;
std::vector<std::shared_ptr<Light>> shadowLights;

//TODO Cleanup
void render_shadows()
{
    static auto tempCamera = Camera::Create("light_camera", 45);
    auto camera = Camera::current();

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Camera::set_current(tempCamera);
    tempCamera->SetProjectionMatrix(glm::mat4(1.0));
    for (auto &light : shadowLights)
    {
        light->render_buffer()->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        tempCamera->SetPosition(light->Position());
        tempCamera->SetViewMatrix(light->TransformMatrix());
        DrawNodesDepth(RenderOpaque);
        light->render_buffer()->bind(false);
    }
    Camera::set_current(camera);
}

double Render::Private::DeltaTime()
{
    return _instance()._deltaTime;
}

void Render::Private::FixedUpdate()
{
    auto InvViewMatrix = glm::inverse(Camera::current()->ViewMatrix());
    auto InvProjMatrix = glm::inverse(Camera::current()->ProjectionMatrix());
    glm::ivec2 res = glm::vec2(Window::size()) * Render::Private::InternalQuality();
    auto index = 0;

    shadowLights.reserve(1000);
    normalLights.reserve(1000);
    shadowLights.clear();
    normalLights.clear();
    while (auto light = Light::Get(index))
    {
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
    while (auto shader = Shader::Get(index))
    {
        shader->use();
        shader->set_uniform("Camera.Position", Camera::current()->Position());
        shader->set_uniform("Camera.Matrix.View", Camera::current()->ViewMatrix());
        shader->set_uniform("Camera.Matrix.Projection", Camera::current()->ProjectionMatrix());
        shader->set_uniform("Camera.InvMatrix.View", InvViewMatrix);
        shader->set_uniform("Camera.InvMatrix.Projection", InvProjMatrix);
        shader->set_uniform("Resolution", glm::vec3(res.x, res.y, res.x / res.y));
        shader->set_uniform("Time", SDL_GetTicks() / 1000.f);
        shader->use(false);
        index++;
    }
}

void Render::Private::SetInternalQuality(float q)
{
    Render::Private::_instance()._internalQuality = q;
}

float Render::Private::InternalQuality()
{
    return Render::Private::_instance()._internalQuality;
}

void Render::Private::RequestRedraw(void)
{
    _instance()._needsUpdate = true;
}

void Render::Private::StartRenderingThread(void)
{
    _instance()._loop = true;
    _instance()._rendering_thread = std::thread(_thread);
}

void Render::Private::StopRenderingThread(void)
{
    _instance()._loop = false;
    _instance()._rendering_thread.join();
    SDL_GL_MakeCurrent(Window::sdl_window(), Window::context());
}

void Render::Private::_thread(void)
{
    float ticks;
    float fixed_timing;

    fixed_timing = SDL_GetTicks() / 1000.f;
    SDL_GL_SetSwapInterval(Engine::SwapInterval());
    SDL_GL_MakeCurrent(Window::sdl_window(), Window::context());
    while (_instance()._loop)
    {
        if (Render::Private::NeedsUpdate())
        {
            Engine::UpdateMutex().lock();
            _instance()._frame_nbr++;
            ticks = SDL_GetTicks() / 1000.f;
            if (ticks - fixed_timing >= 0.015)
            {
                fixed_timing = ticks;
                Render::Private::FixedUpdate();
            }
            Render::Private::Update();
            Render::Private::Scene();
            _instance()._needsUpdate = false;
            Engine::UpdateMutex().unlock();
        }
        //else
        //    Render::Private::Scene();
    }
    SDL_GL_MakeCurrent(Window::sdl_window(), nullptr);
}

uint64_t Render::Private::FrameNbr()
{
    return (_instance()._frame_nbr);
}

void Render::Private::Update()
{
}

void light_pass(std::shared_ptr<Framebuffer> &current_backBuffer, std::shared_ptr<Framebuffer> &current_backTexture, std::shared_ptr<Framebuffer> &current_tbuffertex)
{
    auto lightsPerPass = Config::Get("LightsPerPass", 8u);
    auto shadowsPerPass = Config::Get("ShadowsPerPass", 8u);
    if (lightsPerPass == 0)
        return;
    static auto lighting_shader = GLSL::compile("lighting", lightingFragmentCode, LightingShader,
                                                std::string("\n#define LIGHTNBR ") + std::to_string(lightsPerPass) + std::string("\n#define PointLight ") + std::to_string(Point) + std::string("\n#define DirectionnalLight ") + std::to_string(Directionnal) + std::string("\n"));
    static auto slighting_shader = GLSL::compile("shadow_lighting", lightingFragmentCode, LightingShader,
                                                 (shadowsPerPass > 0 ? std::string("\n#define SHADOW") : std::string("\n")) + std::string("\n#define SHADOWNBR ") + std::to_string(shadowsPerPass) + std::string("\n#define LIGHTNBR ") + std::to_string(lightsPerPass) + std::string("\n#define PointLight ") + std::to_string(Point) + std::string("\n#define DirectionnalLight ") + std::to_string(Directionnal) + std::string("\n"));
    auto actualShadowNbr = std::max(1u, shadowsPerPass);
    auto shader = lighting_shader;
    for (auto i = 0u, j = 0u; i < normalLights.size() || j < shadowLights.size();)
    {
        if (normalLights.size() - i < lightsPerPass && shadowLights.size() - j != 0)
            shader = slighting_shader;
        else
            shader = lighting_shader;
        current_backBuffer->bind();
        shader->use();
        auto lightIndex = 0u;
        while (lightIndex < lightsPerPass && i < normalLights.size())
        {
            auto light = normalLights.at(i);
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Position", light->Position());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Color", light->color() * light->power());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Type", light->type());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].ShadowIndex", -1);
            i++;
            lightIndex++;
        }
        auto shadowIndex = 0u;
        while (lightIndex < lightsPerPass && shadowIndex < actualShadowNbr && j < shadowLights.size())
        {
            auto light = shadowLights.at(j);
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Position", light->Position());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Color", light->color() * light->power());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Type", light->type());
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].ShadowIndex", int(shadowIndex));
            shader->set_uniform("Light[" + std::to_string(lightIndex) + "].Projection", light->TransformMatrix());
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
        Render::Private::DisplayQuad()->draw();
        std::swap(current_backTexture, current_backBuffer);
    }
    shader->use(false);
}

Render::Private &Render::Private::_instance()
{
    static Render::Private *instance = nullptr;
    if (instance == nullptr)
        instance = new Render::Private;
    return *instance;
}

bool Render::Private::NeedsUpdate()
{
    return (_instance()._needsUpdate);
}

#include <Debug.hpp>

void HZBPass(std::shared_ptr<Texture> depthTexture)
{
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_DEPTH_TEST);
    static auto HZBVertexCode =
#include "passthrough.vert"
        ;
    static auto HZBFragmentCode =
#include "hzb.frag"
        ;
    static auto HZBShader = GLSL::compile("HZB", HZBVertexCode, HZBFragmentCode);
    static auto framebuffer = Framebuffer::Create("HZB", depthTexture->size(), 0, 0);
    depthTexture->generate_mipmap();
    depthTexture->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    auto numLevels = 1 + unsigned(floorf(log2f(fmaxf(depthTexture->size().x, depthTexture->size().y))));
    auto currentSize = depthTexture->size();
    for (auto i = 1u; i < numLevels; i++)
    {
        depthTexture->set_parameteri(GL_TEXTURE_BASE_LEVEL, i - 1);
        depthTexture->set_parameteri(GL_TEXTURE_MAX_LEVEL, i - 1);
        currentSize /= 2;
        currentSize = glm::max(currentSize, glm::ivec2(1));
        framebuffer->resize(currentSize);
        //framebuffer->set_attachement(0, depthTexture, i);
        framebuffer->SetDepthBuffer(depthTexture, i);
        framebuffer->bind();
        HZBShader->use();
        HZBShader->bind_texture("in_Texture_Color", depthTexture, GL_TEXTURE0);
        Render::Private::DisplayQuad()->draw();
        HZBShader->use(false);
        framebuffer->SetDepthBuffer(nullptr);
        //framebuffer->set_attachement(0, nullptr);
    }
    depthTexture->set_parameteri(GL_TEXTURE_BASE_LEVEL, 0);
    depthTexture->set_parameteri(GL_TEXTURE_MAX_LEVEL, numLevels - 1);
    depthTexture->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    framebuffer->bind(false);
}

std::shared_ptr<Texture> SSRPass(std::shared_ptr<Framebuffer> gBuffer, std::shared_ptr<Framebuffer> lastRender)
{
    glm::ivec2 res = glm::vec2(Window::size()) * Render::Private::InternalQuality();
    static auto reflectionSteps = Config::Get("ReflexionSteps", 8);
    static auto reflectionSamples = Config::Get("ReflexionSamples", 8);
    static auto reflectionBorderFactor = Config::Get("ReflexionBorderFactor", 10);
    static auto SSRFramebuffer(Framebuffer::Create("SSRFramebuffer", res, 1, 0));
    static auto SSRFramebuffer1(Framebuffer::Create("SSRFramebuffer1", res, 1, 0));
    static auto SSRFramebufferResult(Framebuffer::Create("SSRFramebufferResult", res, 1, 0));
    static auto SSRShader = GLSL::compile("SSR", SSRShaderCode, LightingShader,
                                          std::string("\n#define REFLEXION_STEPS     ") + std::to_string(reflectionSteps)
                                              + std::string("\n#define REFLEXION_SAMPLES ") + std::to_string(reflectionSamples)
                                              + std::string("\n#define SCREEN_BORDER_FACTOR    ") + std::to_string(reflectionBorderFactor)
                                              + std::string("\n"));
    static auto SSRMergeShader = GLSL::compile("SSRMerge", SSRMergeShaderCode, LightingShader);
    static auto SSRBlurShader = GLSL::compile("SSRBlur", SSRBlurShaderCode, LightingShader);
    static std::shared_ptr<Framebuffer> currentFrameBuffer = nullptr;
    SSRFramebufferResult->resize(res);
    currentFrameBuffer = currentFrameBuffer == SSRFramebuffer ? SSRFramebuffer1 : SSRFramebuffer;
    auto lastFrameBuffer = currentFrameBuffer == SSRFramebuffer ? SSRFramebuffer1 : SSRFramebuffer;
    currentFrameBuffer->resize(res);
    currentFrameBuffer->bind();
    SSRShader->use();
    SSRShader->bind_texture("Texture.MaterialValues", gBuffer->attachement(3), GL_TEXTURE0);
    SSRShader->bind_texture("LastColor", lastRender->attachement(0), GL_TEXTURE1);
    SSRShader->bind_texture("LastNormal", lastRender->attachement(2), GL_TEXTURE2);
    SSRShader->bind_texture("LastDepth", gBuffer->depth(), GL_TEXTURE3);
    Render::Private::DisplayQuad()->draw();
    SSRShader->use(false);
    currentFrameBuffer->bind(false);
    //Merge the current result with last result to increase sampling
    SSRFramebufferResult->bind();
    SSRMergeShader->use();
    SSRMergeShader->bind_texture("in_Texture_Color", currentFrameBuffer->attachement(0), GL_TEXTURE1);
    SSRMergeShader->bind_texture("in_Last_Texture_Color", lastFrameBuffer->attachement(0), GL_TEXTURE2);
    Render::DisplayQuad()->draw();
    SSRMergeShader->use(false);
    SSRFramebufferResult->bind(false);
    //Blur the result
    SSRBlurShader->use();
    SSRBlurShader->bind_texture("Texture.MaterialValues", gBuffer->attachement(3), GL_TEXTURE1);
    SSRBlurShader->use(false);
    SSRFramebufferResult->attachement(0)->blur(2, 1, SSRBlurShader);
    return SSRFramebufferResult->attachement(0);
}

void Render::Private::Scene()
{
    if (!Render::Private::NeedsUpdate())
    {
        //present(final_back_buffer);
        return;
    }
    static std::shared_ptr<Texture> brdf;
    if (brdf == nullptr)
    {
        brdf = Texture::Create("brdf", glm::vec2(256, 256), GL_TEXTURE_2D, GL_RG, GL_RG8, GL_UNSIGNED_BYTE, brdfLUT);
        brdf->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        brdf->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glm::ivec2 res = glm::vec2(Window::size()) * Render::Private::InternalQuality();

    auto reflectionSteps = Config::Get("ReflexionSteps", 8);
    auto reflectionSamples = Config::Get("ReflexionSamples", 8);
    auto reflectionBorderFactor = Config::Get("ReflexionBorderFactor", 10);
    static auto temp_buffer = Create_render_buffer("temp_buffer", res);
    static auto temp_buffer1 = Create_render_buffer("temp_buffer1", res);
    static auto back_buffer = Create_back_buffer("back_buffer", res);
    static auto back_buffer1 = Create_back_buffer("back_buffer1", res);
    static auto back_buffer2 = Create_back_buffer("back_buffer2", res);
    static auto final_back_buffer = Create_back_buffer("final_back_buffer", res);
    static auto last_render = Create_back_buffer("last_render", res);
    static auto elighting_shader = GLSL::compile("lighting_env", lightingEnvFragmentCode, LightingShader,
                                                 std::string("\n#define REFLEXION_STEPS		") + std::to_string(reflectionSteps) + std::string("\n#define REFLEXION_SAMPLES	") + std::to_string(reflectionSamples) + std::string("\n#define SCREEN_BORDER_FACTOR	") + std::to_string(reflectionBorderFactor) + std::string("\n"));
    static auto telighting_shader = GLSL::compile("lighting_env_transparent", lightingEnvFragmentCode, LightingShader,
                                                  std::string("\n#define TRANSPARENT") + std::string("\n#define REFLEXION_STEPS		") + std::to_string(reflectionSteps) + std::string("\n#define REFLEXION_SAMPLES	") + std::to_string(reflectionSamples) + std::string("\n#define SCREEN_BORDER_FACTOR ") + std::to_string(reflectionBorderFactor) + std::string("\n"));
    static auto refraction_shader = GLSL::compile("refraction", refractionFragmentCode, LightingShader);

    temp_buffer->resize(res);
    temp_buffer1->resize(res);
    back_buffer->resize(res);
    back_buffer1->resize(res);
    back_buffer2->resize(res);
    final_back_buffer->resize(res);
    last_render->resize(res / 2);

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
    DrawNodes(RenderOpaque);
    std::swap(current_tbuffer, current_tbuffertex);

    glDepthFunc(GL_ALWAYS);
    glDisable(GL_CULL_FACE);
    if (PostTreatments().size() == 0u)
    {
        static auto passthrough_shader = GLSL::compile("passthrough", emptyShaderCode, LightingShader);
        current_tbuffer->bind();
        passthrough_shader->use();
        passthrough_shader->bind_texture("Texture.Albedo", current_tbuffertex->attachement(0), GL_TEXTURE0);
        passthrough_shader->bind_texture("Texture.Emitting", current_tbuffertex->attachement(1), GL_TEXTURE1);
        passthrough_shader->bind_texture("Texture.Specular", current_tbuffertex->attachement(2), GL_TEXTURE2);
        passthrough_shader->bind_texture("Texture.MaterialValues", current_tbuffertex->attachement(3), GL_TEXTURE3);
        passthrough_shader->bind_texture("Texture.AO", current_tbuffertex->attachement(4), GL_TEXTURE4);
        passthrough_shader->bind_texture("Texture.Normal", current_tbuffertex->attachement(5), GL_TEXTURE5);
        passthrough_shader->bind_texture("Texture.Depth", current_tbuffertex->depth(), GL_TEXTURE6);
        Render::Private::DisplayQuad()->draw();
        passthrough_shader->use(false);
    }
    for (auto shader : PostTreatments())
    {
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
        if (Environment::current() != nullptr)
        {
            shaderPtr->bind_texture("Texture.Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE8);
            shaderPtr->bind_texture("Texture.Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE9);
        }
        Render::Private::DisplayQuad()->draw();
        shaderPtr->use(false);

        std::swap(current_tbuffer, current_tbuffertex);
    }
    current_tbuffertex->attachement(4)->blur(1, 0.75);

    auto current_backBuffer = back_buffer;
    auto current_backTexture = back_buffer1;

    current_backTexture->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    light_pass(current_backBuffer, current_backTexture, current_tbuffertex);

    last_render->attachement(0)->generate_mipmap();
    last_render->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    HZBPass(last_render->depth());
    auto SSRResult(SSRPass(current_tbuffertex, last_render));
    //last_render->depth()->generate_mipmap();
    //last_render->depth()->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

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
    elighting_shader->bind_texture("SSRResult", SSRResult, GL_TEXTURE0 + (++i));
    if (Environment::current() != nullptr)
    {
        elighting_shader->bind_texture("Texture.Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE0 + (++i));
        elighting_shader->bind_texture("Texture.Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE0 + (++i));
    }
    Render::Private::DisplayQuad()->draw();
    elighting_shader->use(false);
    std::swap(current_backTexture, current_backBuffer);

    current_tbuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    bool rendered_stuff = false;
    DrawNodes(RenderTransparent);
    std::swap(current_tbuffer, current_tbuffertex);

    if (!rendered_stuff)
    {
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
    telighting_shader->bind_texture("SSRResult", SSRResult, GL_TEXTURE0 + (++i));
    if (Environment::current() != nullptr)
    {
        telighting_shader->bind_texture("Texture.Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE0 + (++i));
        telighting_shader->bind_texture("Texture.Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE0 + (++i));
    }
    Render::Private::DisplayQuad()->draw();
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
    Render::Private::DisplayQuad()->draw();
    refraction_shader->use(false);

    final_back_buffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    final_back_buffer->attachement(1)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    final_back_buffer->attachement(2)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    opaqueBackBuffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    opaqueBackBuffer->attachement(1)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // GENERATE BLOOM FROM out_Brightness
    final_back_buffer->attachement(1)->blur(Config::Get("BloomPass", 1), 3.5);

    static auto passthrough_shader = GLSL::compile("passthrough", passthrough_vertex_code, passthrough_fragment_code);
    passthrough_shader->use();
    last_render->bind();
    passthrough_shader->bind_texture("in_Buffer0", final_back_buffer->attachement(0), GL_TEXTURE0);
    passthrough_shader->bind_texture("in_Buffer1", final_back_buffer->attachement(1), GL_TEXTURE1);
    passthrough_shader->bind_texture("in_Buffer2", final_back_buffer->attachement(2), GL_TEXTURE2);
    passthrough_shader->bind_texture("in_Texture_Depth", final_back_buffer->depth(), GL_TEXTURE6);
    Render::Private::DisplayQuad()->draw();
    passthrough_shader->use(false);
    present(final_back_buffer);
}

std::vector<std::weak_ptr<Shader>> &Render::Private::PostTreatments()
{
    static std::vector<std::weak_ptr<Shader>> ptVector;
    return (ptVector);
}

void Render::Private::AddPostTreatment(std::shared_ptr<Shader> shader)
{
    if (shader != nullptr)
        PostTreatments().push_back(shader);
}

void Render::Private::AddPostTreatment(const std::string &name, const std::string &path)
{
    auto shader = GLSL::parse(name, path, PostShader);

    if (shader != nullptr)
        PostTreatments().push_back(shader);
}

void Render::Private::RemovePostTreatment(std::shared_ptr<Shader> shader)
{
    if (shader != nullptr)
    {
        PostTreatments().erase(std::remove_if(
                                   PostTreatments().begin(),
                                   PostTreatments().end(),
                                   [shader](std::weak_ptr<Shader> p) { return !(p.owner_before(shader) || shader.owner_before(p)); }),
                               PostTreatments().end()); //PostTreatments.erase(shader);
    }
}

void Render::RequestRedraw()
{
    Render::Private::RequestRedraw();
}

void Render::AddPostTreatment(std::shared_ptr<Shader> shader)
{
    Render::Private::AddPostTreatment(shader);
}

void Render::RemovePostTreatment(std::shared_ptr<Shader> shader)
{
    Render::Private::RemovePostTreatment(shader);
}

void Render::Start()
{
    Render::Private::StartRenderingThread();
}

void Render::Stop()
{
    Render::Private::StopRenderingThread();
}

void Render::SetInternalQuality(float q)
{
    Render::Private::SetInternalQuality(q);
}

float Render::InternalQuality()
{
    return Render::Private::InternalQuality();
}

double Render::DeltaTime()
{
    return Render::Private::DeltaTime();
}

const std::shared_ptr<VertexArray> Render::DisplayQuad()
{
    return Render::Private::DisplayQuad();
}
