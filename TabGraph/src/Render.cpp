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
    static double DeltaTime() { return Instance()._deltaTime; }
    static double FixedDeltaTime() { return Instance()._fixedDeltaTime; }
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
    static void SetInternalQuality(float);
    static float InternalQuality();

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
    virtual void _UpdateGPU(float) override;
    virtual void _FixedUpdateGPU(float) override;
    static void _thread();
    std::shared_ptr<Framebuffer> _opaqueBuffer;
    std::shared_ptr<Framebuffer> _lightBuffer;
    std::shared_ptr<Framebuffer> _geometryBuffer;
    double _deltaTime { 0 };
    double _fixedDeltaTime { 0 };
    float _internalQuality { 1 };
    bool _loop { true };
    std::atomic<bool> _needsUpdate { true };
    std::atomic<bool> _drawing { false };
    uint32_t _frame_nbr { 0 };
    std::thread _rendering_thread;
    std::shared_ptr<Texture2D> _brdf;
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
/*
static auto emptyShaderCode =
#include "empty.glsl"
    ;
*/
/*
static auto refractionFragmentCode =
#include "refraction.frag"
    ;
*/


/*
** quad is a singleton
*/

const std::shared_ptr<Geometry> Render::Private::DisplayQuad()
{
    static std::shared_ptr<Geometry> vao;
    if (vao != nullptr) {
        return vao;
    }
    std::vector<glm::vec2> quad(4);
    quad.at(0) = { -1.0f, -1.0f };
    quad.at(1) = { 1.0f, -1.0f };
    quad.at(2) = { -1.0f, 1.0f };
    quad.at(3) = { 1.0f, 1.0f };
    auto accessor(BufferHelper::CreateAccessor(quad));
    vao = Component::Create<Geometry>("DisplayQuad");
    vao->SetDrawingMode(GL_TRIANGLE_STRIP);
    vao->SetAccessor(Geometry::AccessorKey::Position, accessor);
    return vao;
}

void present(std::shared_ptr<Framebuffer> back_buffer)
{
    static std::shared_ptr<Shader> presentShader;
    if (presentShader == nullptr) {
        presentShader = Component::Create<Shader>("present");
        presentShader->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(passthrough_vertex_code, "PassThrough();")));
        presentShader->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(present_fragment_code, "Present();")));
    }
    presentShader->SetTexture("in_Texture_Color", back_buffer->attachement(0));
    presentShader->SetTexture("in_Texture_Emissive", back_buffer->attachement(1));
    presentShader->SetTexture("in_Texture_Depth", back_buffer->depth());
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_CULL_FACE);
    Framebuffer::bind_default();
    presentShader->use();
    Render::Private::DisplayQuad()->Draw();
    presentShader->use(false);
    Window::swap();
}

void Render::Private::SetInternalQuality(float q)
{
    Render::Private::Instance()._internalQuality = q;
}

float Render::Private::InternalQuality()
{
    return Render::Private::Instance()._internalQuality;
}

/*std::shared_ptr<Texture2D> Render::Private::BRDF()
{
    return Render::Private::Instance()._brdf;
}

void Render::Private::SetBRDF(std::shared_ptr<Texture2D> brdf)
{
    Render::Private::Instance()._brdf = brdf;
}*/

void Render::Private::RequestRedraw(void)
{
    Instance()._needsUpdate = true;
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
            Shader::SetGlobalUniform("FrameNumber", Render::Private::FrameNumber());
            Shader::SetGlobalUniform("Camera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
            Shader::SetGlobalUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
            Shader::SetGlobalUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
            Shader::SetGlobalUniform("Camera.InvMatrix.View", glm::inverse(Scene::Current()->CurrentCamera()->ViewMatrix()));
            Shader::SetGlobalUniform("Camera.InvMatrix.Projection", glm::inverse(Scene::Current()->CurrentCamera()->ProjectionMatrix()));
            ticks = SDL_GetTicks() / 1000.0;
            Instance()._frame_nbr++;
            Instance()._deltaTime = ticks - lastTicks;
            lastTicks = ticks;
            Render::Private::Instance().UpdateGPU(Instance()._deltaTime);
            if (ticks - lastTicksFixed >= 0.015) {
                Instance()._fixedDeltaTime = ticks - lastTicksFixed;
                lastTicksFixed = ticks;
                Render::Private::Instance().FixedUpdateGPU(Instance()._fixedDeltaTime);
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

void Render::Private::_UpdateGPU(float delta)
{
    Scene::Current()->UpdateGPU(delta);
}

void Render::Private::_FixedUpdateGPU(float delta)
{
    for (auto& light : Scene::Current()->GetComponents<Light>()) {
        light->render_shadow();
    }
    /*for (auto& light : Scene::Current()->Lights()) {
        light->render_shadow();
    }*/
}

void light_pass(std::shared_ptr<Framebuffer>& lightingBuffer)
{
    glCullFace(GL_FRONT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    lightingBuffer->bind();
    for (auto &light : Scene::Current()->GetComponents<Light>())
        light->Draw();
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

    SSRShader->SetTexture("Texture.Geometry.F0", gBuffer->attachement(2));
    SSRShader->SetTexture("Texture.Geometry.Normal", gBuffer->attachement(4));
    SSRShader->SetTexture("Texture.Geometry.Depth", gBuffer->depth());
    SSRShader->SetTexture("LastColor", lastRender.framebuffer->attachement(0));
    lightBuffer->bind();
    SSRShader->use();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    Render::Private::DisplayQuad()->Draw();
    SSRShader->use(false);
    lightBuffer->bind(false);
    
    
    /*SSRMergeShader->SetUniform("LastViewMatrix", Scene::Current()->CurrentCamera()->ViewMatrix());
    SSRMergeShader->SetUniform("LastProjectionMatrix", Scene::Current()->CurrentCamera()->ProjectionMatrix());
    SSRResultLast = SSRResult;*/
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
    buffer->Create_attachement(GL_RED, GL_R8); //Velocity
    return buffer;
}

auto CreateTransparentMaterialBuffer(const std::string &name, glm::ivec2 res) {
    auto buffer = Component::Create<Framebuffer>(name, res, 0, 1);
    buffer->Create_attachement(GL_RGBA, GL_RGBA16F); // Color;
    buffer->Create_attachement(GL_RED, GL_R8); // Alpha coverage;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); //Emissive
    buffer->Create_attachement(GL_RGB, GL_RGB16F); // Distortion direction XY, roughness
    buffer->Create_attachement(GL_RED, GL_R8); //Velocity
    return buffer;
}

auto CompositingPass(std::shared_ptr<Framebuffer> opaqueBuffer, std::shared_ptr<Framebuffer> transparentBuffer)
{
    static std::shared_ptr<Shader> compositing_shader;
    if (compositing_shader == nullptr) {
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
    compositing_shader->SetTexture("in_OpaqueBuffer0", opaqueBuffer->attachement(0));
    compositing_shader->SetTexture("in_OpaqueBuffer1", opaqueBuffer->attachement(1));
    compositing_shader->SetTexture("in_OpaqueBufferDepth", opaqueBuffer->depth());

    opaqueBuffer->bind();
    compositing_shader->use();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    Render::Private::DisplayQuad()->Draw();
    opaqueBuffer->bind(false);
    compositing_shader->use(false);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

std::shared_ptr<Framebuffer> OpaquePass(const RenderHistory &lastRender)
{
    glm::ivec2 res = Window::size();
    glm::vec2   geometryRes = glm::vec2(std::min(res.x, res.y)) * Render::Private::InternalQuality();
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
    //transparentBuffer1->SetDepthBuffer(transparentBuffer->depth());
    //transparentBuffer->SetDepthBuffer(opaqueBuffer->depth());
    

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

    if (Config::Get("SSRQuality", 4) > 0)
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
    glPolygonOffset(1.0, 1.0);

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
    //glEnable(GL_DEPTH_TEST);
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
    glm::ivec2 res = Window::size();
    //Setup framebuffers
    //static auto finalRenderBuffer(CreateLightingBuffer("finalRenderBuffer", res));
    static auto brightnessBuffer(Component::Create<Framebuffer>("lastRender", res, 1, 0));
    //static auto lastRender(CreateOpaqueMaterialBuffer(glm::ivec2(std::min(res.x, res.y) * 0.5)));
    static std::array<RenderHistory, 8> renderHistory;
    if (renderHistory.at(0).framebuffer == nullptr)
        renderHistory.at(0).framebuffer = CreateOpaqueMaterialBuffer(glm::ivec2(std::min(res.x, res.y) * 0.5));
    //Setup shaders
    static std::shared_ptr<Shader> passthrough_shader;
    if (passthrough_shader == nullptr) {
        passthrough_shader = Component::Create<Shader>("passthrough");
        passthrough_shader->SetStage(Component::Create<ShaderStage>(GL_VERTEX_SHADER, Component::Create<ShaderCode>(passthrough_vertex_code, "PassThrough();")));
        passthrough_shader->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(passthrough_fragment_code, "PassThrough();")));
        Render::Private::Instance().AddComponent(passthrough_shader);
    }

    auto renderBuffer(OpaquePass(renderHistory.at(0)));

    //auto transpRenderBuffer(TranspPass(lastRender, renderBuffer));
    //renderBuffer->attachement(1)->blur(Config::Get("BloomPass", 1), 3.5);

    static std::shared_ptr<Shader> TemporalMergeShader;
    if (TemporalMergeShader == nullptr) {
        auto TemporalMergeShaderCode =
#include "TemporalAccumulation.frag"
            ;
        TemporalMergeShader = Component::Create<Shader>("TemporalAccumulation", Shader::Type::LightingShader);
        TemporalMergeShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(Component::Create<ShaderCode>(TemporalMergeShaderCode, "TemporalAccumulation();"));
        Render::Private::Instance().AddComponent(TemporalMergeShader);
    }

    renderBuffer->bind();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    for (auto i = 0; i < renderHistory.size(); ++i)
    {
        if (renderHistory.at(i).framebuffer == nullptr)
            continue;
        TemporalMergeShader->SetTexture("in_renderHistory[" + std::to_string(i) + "].color", renderHistory.at(i).framebuffer->attachement(0));
        TemporalMergeShader->SetTexture("in_renderHistory[" + std::to_string(i) + "].emissive", renderHistory.at(i).framebuffer->attachement(1));
        TemporalMergeShader->SetTexture("in_renderHistory[" + std::to_string(i) + "].velocity", renderHistory.at(i).framebuffer->attachement(2));
        //TemporalMergeShader->SetTexture("in_renderHistory[" + std::to_string(i) + "].depth", renderHistory.at(i).framebuffer->depth());
        TemporalMergeShader->SetUniform("in_renderHistory[" + std::to_string(i) + "].viewMatrix", renderHistory.at(i).viewMatrix);
        TemporalMergeShader->SetUniform("in_renderHistory[" + std::to_string(i) + "].projectionMatrix", renderHistory.at(i).projectionMatrix);
    }
    TemporalMergeShader->SetTexture("Texture.Geometry.Depth", renderBuffer->depth());
    TemporalMergeShader->use();
    Render::DisplayQuad()->Draw();
    TemporalMergeShader->use(false);
    renderBuffer->bind(false);
    

    RenderHistory temp = renderHistory.at(renderHistory.size() - 1), temp1;
    for (int i = 0; i < renderHistory.size(); i++) {
        temp1 = renderHistory.at(i);
        renderHistory.at(i) = temp;
        temp = temp1;
    }
    if (renderHistory.at(0).framebuffer == nullptr)
        renderHistory.at(0).framebuffer = CreateOpaqueMaterialBuffer(renderBuffer->Size());

    //lastRender->Resize(glm::ivec2(std::min(renderBuffer->Size().x, renderBuffer->Size().y) * 0.5));
    renderHistory.at(0).framebuffer->Resize(renderBuffer->Size());
    renderHistory.at(0).viewMatrix = Scene::Current()->CurrentCamera()->ViewMatrix();
    renderHistory.at(0).projectionMatrix = Scene::Current()->CurrentCamera()->ProjectionMatrix();
    //renderBuffer->BlitTo(lastRender, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    //lastRender->attachement(0)->generate_mipmap();
    //lastRender->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_DEPTH_TEST);
    renderHistory.at(0).framebuffer->bind();
    passthrough_shader->SetTexture("in_Buffer0", renderBuffer->attachement(0));
    passthrough_shader->SetTexture("in_Buffer1", renderBuffer->attachement(1));
    passthrough_shader->SetTexture("in_Buffer2", renderBuffer->attachement(2));
    passthrough_shader->SetTexture("in_Texture_Depth", renderBuffer->depth());
    passthrough_shader->use();
    Render::Private::DisplayQuad()->Draw();
    passthrough_shader->use(false);
    renderHistory.at(0).framebuffer->bind(false);
    renderHistory.at(0).framebuffer->attachement(0)->generate_mipmap();
    renderHistory.at(0).framebuffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glDepthMask(GL_FALSE);

    renderBuffer->attachement(1)->blur(Config::Get("BloomPass", 1), 3.5);
    present(renderBuffer);
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

/*void Render::Private::AddPostTreatment(const std::string &name, const std::string &path)
{
    auto shader = GLSL::parse(name, path, PostShader);

    if (shader != nullptr)
        PostTreatments().push_back(shader);
}*/

void Render::Private::RemovePostTreatment(std::shared_ptr<Shader> shader)
{
    /*if (shader != nullptr) {
        PostTreatments().erase(std::remove_if(
                                   PostTreatments().begin(),
                                   PostTreatments().end(),
                                   [shader](std::shared_ptr<Shader> p) { return !(p.owner_before(shader) || shader.owner_before(p)); }),
            PostTreatments().end()); //PostTreatments.erase(shader);
    }*/
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

double Render::FixedDeltaTime()
{
    return Render::Private::FixedDeltaTime();
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