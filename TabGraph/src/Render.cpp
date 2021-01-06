/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-10-19 14:08:09
*/

#include "Render.hpp"
#include "Buffer/BufferHelper.hpp"
#include "Camera/Camera.hpp" // for Camera
#include "Config.hpp" // for Config
#include "Engine.hpp" // for UpdateMutex, SwapInterval
#include "Environment.hpp" // for Environment
#include "Framebuffer.hpp" // for Framebuffer
#include "Light/Light.hpp" // for Light, Directionnal, Point
#include "Mesh/Geometry.hpp" // for Geometry
#include "Parser/GLSL.hpp" // for GLSL, LightingShader, PostShader
#include "Scene/Scene.hpp"
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Cubemap.hpp"
#include "Texture/Texture2D.hpp" // for Texture2D
#include "Transform.hpp"
#include "Window.hpp" // for Window
//#include "brdfLUT.hpp" // for brdfLUT
#include <GL/glew.h> // for GL_TEXTURE0, glDepthFunc, glClear, glDis...
#include <SDL_timer.h> // for SDL_GetTicks
#include <SDL_video.h> // for SDL_GL_MakeCurrent, SDL_GL_SetSwapInterval
#include <algorithm> // for max, remove_if
#include <atomic> // for atomic
#include <glm/glm.hpp> // for glm::inverse, vec2_scale, vec3_scale
#include <iostream> // for char_traits, endl, cout, operator<<, ost...
#include <mutex> // for mutex
#include <stdint.h> // for uint64_t, uint16_t
#include <string> // for operator+, to_string, string
#include <thread> // for thread
#include <vector> // for vector<>::iterator, vector

namespace Render {
class Private : public Component {
    //class Render::Private {
public:
    static Render::Private &Instance();
    static void Scene();
    static void AddPostTreatment(std::shared_ptr<Shader>);
    static void AddPostTreatment(const std::string& name, const std::string& path);
    static void RemovePostTreatment(std::shared_ptr<Shader>);
    static void StartRenderingThread();
    static void StopRenderingThread();
    static void RequestRedraw();
    static std::atomic<bool>& NeedsUpdate();
    static std::atomic<bool>& Drawing();
    static uint32_t FrameNumber(void);
    static void SetOpaqueBuffer(const std::shared_ptr<Framebuffer>&);
    static const std::shared_ptr<Framebuffer> OpaqueBuffer();
    static void SetLightBuffer(const std::shared_ptr<Framebuffer>&);
    static const std::shared_ptr<Framebuffer> LightBuffer();
    static void SetGeometryBuffer(const std::shared_ptr<Framebuffer>&);
    static const std::shared_ptr<Framebuffer> GeometryBuffer();
    static const std::shared_ptr<Geometry> DisplayQuad();
    static std::vector<std::shared_ptr<Shader>>& PostTreatments();

    static Signal<float>& OnFixedUpdate() { return Instance()._onFixedUpdate; };
    static Signal<float>& OnUpdate() { return Instance()._onUpdate; };

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return nullptr;
        //return Component::Create<Private>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _UpdateCPU(float) override {};
    virtual void _FixedUpdateCPU(float) override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    static void _thread();
    std::shared_ptr<Framebuffer> _opaqueBuffer;
    std::shared_ptr<Framebuffer> _lightBuffer;
    std::shared_ptr<Framebuffer> _geometryBuffer;
    bool _loop { true };
    std::atomic<bool> _needsUpdate { true };
    std::atomic<bool> _drawing { false };
    uint32_t _frame_nbr { 0 };
    std::thread _rendering_thread;
    std::shared_ptr<Texture2D> _brdf;
    Signal<float> _onFixedUpdate;
    Signal<float> _onUpdate;
};
} // namespace Render


// quad is a singleton
const std::shared_ptr<Geometry> Render::Private::DisplayQuad()
{
    static std::shared_ptr<Geometry> vao;
    if (vao != nullptr) {
        return vao;
    }
    auto accessor(BufferHelper::CreateAccessor<glm::vec2>(3));
    vao = Component::Create<Geometry>("DisplayQuad");
    vao->SetDrawingMode(GL_TRIANGLES);
    vao->SetAccessor(Geometry::AccessorKey::Position, accessor);
    return vao;
}

void Render::Private::RequestRedraw(void)
{
    Instance()._needsUpdate = true;
}

void Render::Private::RemovePostTreatment(std::shared_ptr<Shader>)
{
    //TODO : re-implement this
}

void Render::Private::StartRenderingThread(void)
{
    Instance()._loop = true;
    Instance()._rendering_thread = std::thread(_thread);
}

void Render::Private::StopRenderingThread(void)
{
    Instance()._loop = false;
    Instance()._rendering_thread.join();
    SDL_GL_MakeCurrent(Window::sdl_window(), Window::context());
}

void Render::Private::_thread(void)
{
    double ticks;
    double lastTicks;
    double lastTicksFixed = lastTicks = SDL_GetTicks() / 1000.0;

    SDL_GL_MakeCurrent(Window::sdl_window(), Window::context());
    SDL_GL_SetSwapInterval(Engine::SwapInterval());
    while (Instance()._loop) {
        if (Render::Private::NeedsUpdate()) {
            ticks = SDL_GetTicks() / 1000.0;
            Instance()._frame_nbr++;
            Shader::SetGlobalUniform("FrameNumber", Render::Private::FrameNumber());
            Shader::SetGlobalUniform("Camera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
            Shader::SetGlobalUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
            Shader::SetGlobalUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
            Shader::SetGlobalUniform("Camera.InvMatrix.View", glm::inverse(Scene::Current()->CurrentCamera()->ViewMatrix()));
            Shader::SetGlobalUniform("Camera.InvMatrix.Projection", glm::inverse(Scene::Current()->CurrentCamera()->ProjectionMatrix()));
            Render::Private::Instance().OnUpdate()(ticks - lastTicks);
            lastTicks = ticks;
            if (ticks - lastTicksFixed >= 0.015) {
                Render::Private::Instance().OnFixedUpdate()(ticks - lastTicksFixed);
                lastTicksFixed = ticks;
            }
            Instance()._drawing = true;
            Render::Private::Scene();
            Instance()._drawing = false;
            Instance()._needsUpdate = false;
            Shader::SetGlobalUniform("PrevCamera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
            Shader::SetGlobalUniform("PrevCamera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
            Shader::SetGlobalUniform("PrevCamera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
            Shader::SetGlobalUniform("PrevCamera.InvMatrix.View", glm::inverse(Scene::Current()->CurrentCamera()->ViewMatrix()));
            Shader::SetGlobalUniform("PrevCamera.InvMatrix.Projection", glm::inverse(Scene::Current()->CurrentCamera()->ProjectionMatrix()));
        }
    }
    SDL_GL_MakeCurrent(Window::sdl_window(), nullptr);
}

uint32_t Render::Private::FrameNumber()
{
    return (Instance()._frame_nbr);
}

void Render::Private::SetOpaqueBuffer(const std::shared_ptr<Framebuffer>& fb)
{
    Instance()._opaqueBuffer = fb;
}

const std::shared_ptr<Framebuffer> Render::Private::OpaqueBuffer()
{
    return Instance()._opaqueBuffer;
}

void Render::Private::SetLightBuffer(const std::shared_ptr<Framebuffer>& fb)
{
    Instance()._lightBuffer = fb;
}

const std::shared_ptr<Framebuffer> Render::Private::LightBuffer()
{
    return Instance()._lightBuffer;
}

void Render::Private::SetGeometryBuffer(const std::shared_ptr<Framebuffer>& fb)
{
    Instance()._geometryBuffer = fb;
}

const std::shared_ptr<Framebuffer> Render::Private::GeometryBuffer()
{
    return Instance()._geometryBuffer;
}

Render::Private &Render::Private::Instance()
{
    static auto instance = Component::Create<Render::Private>();
    return *instance;
}

void light_pass(std::shared_ptr<Framebuffer>& lightingBuffer)
{
    for (auto& light : Scene::Current()->GetComponents<Light>()) {
        light->render_shadow();
        glCullFace(GL_FRONT);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        lightingBuffer->bind();
        light->Draw();
    }
    lightingBuffer->bind(false);
    glCullFace(GL_BACK);
}

std::atomic<bool>& Render::Private::NeedsUpdate()
{
    return Instance()._needsUpdate;
}

std::atomic<bool>& Render::Private::Drawing()
{
    return (Instance()._drawing);
}

#include <Debug.hpp>

void HZBPass(std::shared_ptr<Texture2D> depthTexture)
{
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_DEPTH_TEST);
    static auto PassThroughVertexCode =
#include "passthrough.vert"
        ;
    static auto HZBFragmentCode =
#include "hzb.frag"
        ;
    static std::shared_ptr<Shader> HZBShader;
    if (HZBShader == nullptr) {
        auto ShaderVertexCode = Component::Create<ShaderCode>();
        ShaderVertexCode->SetCode(PassThroughVertexCode);
        ShaderVertexCode->SetTechnique("PassThrough();");
        auto ShaderFragmentCode = Component::Create<ShaderCode>();
        ShaderFragmentCode->SetCode(HZBFragmentCode);
        ShaderFragmentCode->SetTechnique("HZB();");
        HZBShader = Component::Create<Shader>("HZB");
        HZBShader->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, ShaderVertexCode));
        HZBShader->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, ShaderFragmentCode));
    }
    static auto framebuffer = Component::Create<Framebuffer>("HZB", depthTexture->Size(), 0, 0);
    depthTexture->generate_mipmap();
    depthTexture->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    auto numLevels = 1 + unsigned(floorf(log2f(fmaxf(depthTexture->Size().x, depthTexture->Size().y))));
    auto currentSize = depthTexture->Size();
    for (auto i = 1u; i < numLevels; i++) {
        depthTexture->set_parameteri(GL_TEXTURE_BASE_LEVEL, i - 1);
        depthTexture->set_parameteri(GL_TEXTURE_MAX_LEVEL, i - 1);
        currentSize /= 2;
        currentSize = glm::max(currentSize, glm::ivec2(1));
        framebuffer->Resize(currentSize);
        //framebuffer->set_attachement(0, depthTexture, i);
        framebuffer->SetDepthBuffer(depthTexture, i);
        framebuffer->bind();
        HZBShader->SetTexture("in_Texture_Color", depthTexture);
        HZBShader->use();
        Render::Private::DisplayQuad()->Draw();
        HZBShader->use(false);
        framebuffer->SetDepthBuffer(nullptr);
        //framebuffer->set_attachement(0, nullptr);
    }
    depthTexture->set_parameteri(GL_TEXTURE_BASE_LEVEL, 0);
    depthTexture->set_parameteri(GL_TEXTURE_MAX_LEVEL, numLevels - 1);
    depthTexture->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    framebuffer->bind(false);
}

void SSAOPass(std::shared_ptr<Framebuffer> gBuffer)
{
    glm::ivec2 res = gBuffer->Size(); // glm::vec2(1024) * Render::Private::InternalQuality();// *Config::Get("SSAOResolutionFactor", 0.5f);
    glDepthMask(false);
    //static auto SSAOResult(Component::Create<Framebuffer>("SSAOBuffer", res, 1, 0));
    static std::shared_ptr<Shader> SSAOShader;
    if (SSAOShader == nullptr) {
        auto SSAOFragmentCode =
        #include "ssao.frag"
            ;
        SSAOShader = Component::Create<Shader>("SSAO", Shader::Type::PostShader);
        SSAOShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(SSAOFragmentCode, "SSAO();"));
        Render::Private::Instance().AddComponent(SSAOShader);
    }
    SSAOShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SSAO_QUALITY", std::to_string(Config::Get("SSAOQuality", 4)));
    //SSAOShader->SetTexture("Texture.Geometry.CDiff", gBuffer->attachement(0));
    SSAOShader->SetTexture("Texture.Geometry.Normal", gBuffer->attachement(4));
    SSAOShader->SetTexture("Texture.Geometry.Depth", gBuffer->depth());
    gBuffer->bind();
    SSAOShader->use();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    Render::DisplayQuad()->Draw();
    SSAOShader->use(false);
    gBuffer->bind(false);
}

float ComputeRoughnessMaskScale()
{
    float MaxRoughness = std::clamp(Config::Get("SSRMaxRoughness", 0.8f), 0.01f, 1.0f);

    float RoughnessMaskScale = -2.0f / MaxRoughness;
    return RoughnessMaskScale * (Config::Get("SSRQuality", 4) < 3 ? 2.0f : 1.0f);
}

struct RenderHistory {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    std::shared_ptr<Framebuffer> framebuffer;
};

auto SSRPass(std::shared_ptr<Framebuffer> gBuffer, const RenderHistory &lastRender, std::shared_ptr<Framebuffer> lightBuffer)
{
    static auto SSRBuffer = Component::Create<Framebuffer>("SSRBuffer", lightBuffer->Size(), 1);
    SSRBuffer->Resize(lightBuffer->Size());
    static std::shared_ptr<Shader> SSRShader;
    if (SSRShader == nullptr) {
        auto SSRFragmentCode =
            #include "SSR.frag"
            ;
        SSRShader = Component::Create<Shader>("SSR0", Shader::Type::LightingShader);
        SSRShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(SSRFragmentCode, "SSR();"));
        Render::Private::Instance().AddComponent(SSRShader);
    }
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SSR_QUALITY", std::to_string(Config::Get("SSRQuality", 4)));
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SCREEN_BORDER_FACTOR", std::to_string(Config::Get("SSRBorderFactor", 10)));
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("ROUGHNESSMASKSCALE", std::to_string(ComputeRoughnessMaskScale()));

    HZBPass(gBuffer->depth());
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    SSRShader->SetTexture("Texture.Geometry.CDiff", gBuffer->attachement(0));
    SSRShader->SetTexture("Texture.Geometry.F0", gBuffer->attachement(2));
    SSRShader->SetTexture("Texture.Geometry.Normal", gBuffer->attachement(4));
    SSRShader->SetTexture("Texture.Geometry.Depth", gBuffer->depth());
    SSRShader->SetTexture("LastColor", lastRender.framebuffer->attachement(0));
    lightBuffer->bind();
    SSRShader->use();
    Render::Private::DisplayQuad()->Draw();
    SSRShader->use(false);
    lightBuffer->bind(false);
    glDisable(GL_BLEND);

    //lightBuffer->attachement(1)->blur(1, 1.5);
}

std::shared_ptr<Framebuffer> CreateGeometryBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Component::Create<Framebuffer>(name, size, 0, 1);

    buffer->Create_attachement(GL_RGBA, GL_RGBA8); // BRDF CDiff, Transparency;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emissive;
    buffer->Create_attachement(GL_RGBA, GL_RGBA8); // BRDF F0, BRDF Alpha;
    buffer->Create_attachement(GL_RED, GL_R8); //AO
    buffer->Create_attachement(GL_RGB, GL_RGB8_SNORM); // Normal;
    auto idTexture = buffer->Create_attachement(GL_RED_INTEGER, GL_R32UI);
    idTexture->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    idTexture->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    buffer->Create_attachement(GL_RED, GL_R8); //Velocity
    return (buffer);
}

auto CreateHistoryBuffer(glm::ivec2 res) {
    auto buffer = Component::Create<Framebuffer>("HistoryBuffer", res, 1, 0);
    //buffer->Create_attachement(GL_RGB, GL_RGB8); //Color
    //buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); //Emissive
    return buffer;
}

std::shared_ptr<Framebuffer> CreateLightingBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Component::Create<Framebuffer>(name, size, 0, 0);
    buffer->Create_attachement(GL_RGBA, GL_RGBA16F); // Diffuse, Specular Luminance;
    //buffer->Create_attachement(GL_RGB, GL_RGB16F); // Specular;
    buffer->Create_attachement(GL_RGBA, GL_RGBA8); // Reflection;
    return (buffer);
}

auto CreateOpaqueMaterialBuffer(glm::ivec2 res) {
    auto buffer = Component::Create<Framebuffer>("OpaqueMaterialBuffer", res, 0, 1);
    buffer->Create_attachement(GL_RGB, GL_RGB8); //Color
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); //Emissive
    //buffer->Create_attachement(GL_RED, GL_R8);
    return buffer;
}

auto CreateTransparentMaterialBuffer(const std::string &name, glm::ivec2 res) {
    auto buffer = Component::Create<Framebuffer>(name, res, 0, 1);
    buffer->Create_attachement(GL_RGBA, GL_RGBA16F); // Color;
    buffer->Create_attachement(GL_RED, GL_R8); // Alpha coverage;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); //Emissive
    buffer->Create_attachement(GL_RGB, GL_RGB8); // Distortion direction XY, roughness
    return buffer;
}

auto CompositingPass(std::shared_ptr<Framebuffer> opaqueBuffer, std::shared_ptr<Framebuffer> transparentBuffer)
{
    static std::shared_ptr<Shader> compositing_shader;
    if (compositing_shader == nullptr) {
        auto passthrough_vertex_code =
#include "passthrough.vert"
            ;
        auto compositingShaderCode =
#include "compositing.frag"
            ;
        compositing_shader = Component::Create<Shader>("compositing", Shader::Type::LightingShader);
        compositing_shader->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(passthrough_vertex_code, "PassThrough();")));
        compositing_shader->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(compositingShaderCode, "Composite();")));
        Render::Private::Instance().AddComponent(compositing_shader);
    }

    compositing_shader->SetTexture("in_TransparentBuffer0", transparentBuffer->attachement(0));
    compositing_shader->SetTexture("in_TransparentBuffer1", transparentBuffer->attachement(1));
    compositing_shader->SetTexture("in_TransparentBuffer2", transparentBuffer->attachement(2));
    compositing_shader->SetTexture("in_TransparentBuffer3", transparentBuffer->attachement(3));
    compositing_shader->SetTexture("in_TransparentDepth", transparentBuffer->depth());
    compositing_shader->SetTexture("in_OpaqueBuffer0", opaqueBuffer->attachement(0));
    compositing_shader->SetTexture("in_OpaqueBuffer1", opaqueBuffer->attachement(1));
    //compositing_shader->SetTexture("in_OpaqueBufferDepth", opaqueBuffer->depth());

    opaqueBuffer->bind();
    compositing_shader->use();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    Render::Private::DisplayQuad()->Draw();
    opaqueBuffer->bind(false);
    compositing_shader->use(false);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

std::shared_ptr<Framebuffer> OpaquePass(const RenderHistory &lastRender)
{
    glm::ivec2 res = glm::vec2(Window::size()) * Config::Get("InternalQuality", 1.f);
    glm::vec2   geometryRes = glm::vec2(std::min(res.x, res.y));
    static auto geometryBuffer = CreateGeometryBuffer("GeometryBuffer", geometryRes);
    static auto lightingBuffer = CreateLightingBuffer("LightingBuffer", geometryRes);
    static auto opaqueBuffer = CreateOpaqueMaterialBuffer(res);
    static auto transparentBuffer = CreateTransparentMaterialBuffer("TransparentBuffer0", res);
    static auto transparentBuffer1 = CreateTransparentMaterialBuffer("TransparentBuffer1", res / 2);
    Render::Private::SetOpaqueBuffer(opaqueBuffer);
    Render::Private::SetLightBuffer(lightingBuffer);
    Render::Private::SetGeometryBuffer(geometryBuffer);
    geometryBuffer->Resize(geometryRes);
    lightingBuffer->Resize(geometryRes);
    opaqueBuffer->Resize(res);
    transparentBuffer->Resize(res);
    transparentBuffer1->Resize(res / 2);

    geometryBuffer->bind();
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Scene::Current()->Render(RenderPass::Geometry, RenderMod::RenderAll);
    geometryBuffer->bind(false);

    lightingBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    light_pass(lightingBuffer);
    lightingBuffer->bind(false);

    if (Config::Get("SSRQuality", 4) > 0 && lastRender.framebuffer != nullptr)
        SSRPass(geometryBuffer, lastRender, lightingBuffer);
    if (Config::Get("SSAOQuality", 4) > 0)
        SSAOPass(geometryBuffer);

    opaqueBuffer->bind();
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    if (Scene::Current()->GetEnvironment() != nullptr) {
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        Scene::Current()->GetEnvironment()->Draw();
    }
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    Scene::Current()->Render(RenderPass::Material, RenderMod::RenderOpaque);

    auto zero = glm::vec4(0);
    auto one = glm::vec4(1);

    transparentBuffer->bind();
    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(2, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(3, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearBufferfv(GL_COLOR, 0, &zero[0]);
    glClearBufferfv(GL_COLOR, 1, &one[0]);
    glClearBufferfv(GL_COLOR, 2, &zero[0]);
    glClearBufferfv(GL_COLOR, 3, &zero[0]);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    Scene::Current()->Render(RenderPass::Material, RenderMod::RenderTransparent);
    
    transparentBuffer1->bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    transparentBuffer1->bind(false);

    transparentBuffer->BlitTo(transparentBuffer1, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(2.0, 2.0);

    transparentBuffer1->bind();
    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(2, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(3, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearBufferfv(GL_COLOR, 0, &zero[0]);
    glClearBufferfv(GL_COLOR, 1, &one[0]);
    glClearBufferfv(GL_COLOR, 2, &zero[0]);
    glClearBufferfv(GL_COLOR, 3, &zero[0]);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_SRC_ALPHA);
    glBlendFunci(2, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    glBlendFunci(3, GL_ONE, GL_ONE);
    glColorMaski(3, GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_GREATER);
    glEnable(GL_BLEND);
    Scene::Current()->Render(RenderPass::Material, RenderMod::RenderTransparent);
    transparentBuffer1->bind(false);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(2, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(3, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    opaqueBuffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    opaqueBuffer->attachement(1)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    opaqueBuffer->attachement(0)->generate_mipmap();
    opaqueBuffer->attachement(1)->generate_mipmap();
    opaqueBuffer->attachement(0)->set_parameteri(GL_TEXTURE_BASE_LEVEL, 1);
    opaqueBuffer->attachement(1)->set_parameteri(GL_TEXTURE_BASE_LEVEL, 1);
    CompositingPass(opaqueBuffer, transparentBuffer1);
    opaqueBuffer->attachement(0)->set_parameteri(GL_TEXTURE_BASE_LEVEL, 0);
    opaqueBuffer->attachement(1)->set_parameteri(GL_TEXTURE_BASE_LEVEL, 0);
    opaqueBuffer->attachement(0)->generate_mipmap();
    opaqueBuffer->attachement(1)->generate_mipmap();
    opaqueBuffer->attachement(0)->set_parameteri(GL_TEXTURE_BASE_LEVEL, 1);
    opaqueBuffer->attachement(1)->set_parameteri(GL_TEXTURE_BASE_LEVEL, 1);
    CompositingPass(opaqueBuffer, transparentBuffer);
    opaqueBuffer->attachement(0)->set_parameteri(GL_TEXTURE_BASE_LEVEL, 0);
    opaqueBuffer->attachement(1)->set_parameteri(GL_TEXTURE_BASE_LEVEL, 0);

    return opaqueBuffer;
}

void Render::Private::Scene()
{
    if (Scene::Current() == nullptr || Scene::Current()->CurrentCamera() == nullptr) {
        //present(final_back_buffer);
        return;
    }
    glm::ivec2 res = glm::vec2(Window::size()) * Config::Get("InternalQuality ", 1.f);
    static std::array<RenderHistory, 1> renderHistory;
    auto renderBuffer(OpaquePass(renderHistory.at(0)));
    renderBuffer->attachement(1)->blur(Config::Get("BloomPass", 1), 5);

    static std::shared_ptr<Shader> passthroughShader;
    if (passthroughShader == nullptr) {
        auto passthrough_vertex_code =
#include "passthrough.vert"
            ;
        auto present_fragment_code =
#include "passthrough.frag"
            ;
        passthroughShader = Component::Create<Shader>("passthrough");
        passthroughShader->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(passthrough_vertex_code, "PassThrough();")));
        passthroughShader->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(present_fragment_code, "PassThrough();")));
    }
    passthroughShader->SetTexture("in_Buffer0", renderBuffer->attachement(1));
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    renderBuffer->bind();
    passthroughShader->use();
    Render::Private::DisplayQuad()->Draw();
    renderBuffer->bind(false);
    passthroughShader->use(false);

    static std::shared_ptr<Shader> TemporalAccumulationShader;
    if (TemporalAccumulationShader == nullptr) {
        auto TemporalAccumulationShaderCode =
#include "TemporalAccumulation.frag"
            ;
        TemporalAccumulationShader = Component::Create<Shader>("TemporalAccumulation", Shader::Type::LightingShader);
        TemporalAccumulationShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(TemporalAccumulationShaderCode, "TemporalAccumulation();"));
        Render::Private::Instance().AddComponent(TemporalAccumulationShader);
    }

    static auto finalRenderBuffer(Component::Create<Framebuffer>("finalRenderBuffer", renderBuffer->Size(), 1, 0));
    finalRenderBuffer->Resize(Window::size());
    finalRenderBuffer->bind();
    glDisable(GL_BLEND);
    TemporalAccumulationShader->SetDefine("HISTORYSIZE", std::to_string(renderHistory.size()));
    for (auto i = 0; i < renderHistory.size(); ++i)
    {
        if (renderHistory.at(i).framebuffer == nullptr)
            continue;
        TemporalAccumulationShader->SetTexture("in_renderHistory[" + std::to_string(i) + "].color", renderHistory.at(i).framebuffer->attachement(0));
        //TemporalAccumulationShader->SetTexture("in_renderHistory[" + std::to_string(i) + "].emissive", renderHistory.at(i).framebuffer->attachement(1));
        TemporalAccumulationShader->SetUniform("in_renderHistory[" + std::to_string(i) + "].viewMatrix", renderHistory.at(i).viewMatrix);
        TemporalAccumulationShader->SetUniform("in_renderHistory[" + std::to_string(i) + "].projectionMatrix", renderHistory.at(i).projectionMatrix);
    }
    TemporalAccumulationShader->SetTexture("in_CurrentColor", renderBuffer->attachement(0));
    TemporalAccumulationShader->SetTexture("in_VelocityTexture", Render::GeometryBuffer()->attachement(6));
    TemporalAccumulationShader->SetTexture("Texture.Geometry.Depth", renderBuffer->depth());
    TemporalAccumulationShader->use();
    Render::DisplayQuad()->Draw();
    TemporalAccumulationShader->use(false);
    finalRenderBuffer->bind(false);

    RenderHistory temp = renderHistory.at(renderHistory.size() - 1), temp1;
    for (int i = 0; i < renderHistory.size(); i++) {
        temp1 = renderHistory.at(i);
        renderHistory.at(i) = temp;
        temp = temp1;
    }
    if (renderHistory.at(0).framebuffer == nullptr)
        renderHistory.at(0).framebuffer = CreateHistoryBuffer(renderBuffer->Size());
    renderHistory.at(0).framebuffer->Resize(renderBuffer->Size());
    renderHistory.at(0).viewMatrix = Scene::Current()->CurrentCamera()->ViewMatrix();
    renderHistory.at(0).projectionMatrix = Scene::Current()->CurrentCamera()->ProjectionMatrix();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_DEPTH_TEST);
    finalRenderBuffer->BlitTo(renderHistory.at(0).framebuffer, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glDepthMask(GL_FALSE);
    
    finalRenderBuffer->BlitTo(nullptr, GL_COLOR_BUFFER_BIT);
    Window::swap();
    glFinish();
}

std::vector<std::shared_ptr<Shader>>& Render::Private::PostTreatments()
{
    static std::vector<std::shared_ptr<Shader>> ptVector;
    return (ptVector);
}

void Render::Private::AddPostTreatment(std::shared_ptr<Shader> shader)
{
    if (shader != nullptr)
        PostTreatments().push_back(shader);
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

std::atomic<bool>& Render::NeedsUpdate()
{
    return Render::Private::NeedsUpdate();
}

std::atomic<bool>& Render::Drawing()
{
    return Render::Private::Drawing();
}

const std::shared_ptr<Geometry> Render::DisplayQuad()
{
    return Render::Private::DisplayQuad();
}

Signal<float>& Render::OnFixedUpdate()
{
    return Render::Private::OnFixedUpdate();
}

Signal<float>& Render::OnUpdate()
{
    return Render::Private::OnUpdate();
}

const std::shared_ptr<Framebuffer> Render::OpaqueBuffer()
{
    return Render::Private::OpaqueBuffer();
}

const std::shared_ptr<Framebuffer> Render::LightBuffer()
{
    return Render::Private::LightBuffer();
}

const std::shared_ptr<Framebuffer> Render::GeometryBuffer()
{
    return Render::Private::GeometryBuffer();
}

uint32_t Render::FrameNumber()
{
    return Render::Private::FrameNumber();
}
