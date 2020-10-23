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
#include "brdfLUT.hpp" // for brdfLUT
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
    static std::shared_ptr<Render::Private> Create();
    static std::shared_ptr<Render::Private> Instance();
    //static void Update(float delta);
    //static void FixedUpdate(float delta);
    static void Scene();
    static void AddPostTreatment(std::shared_ptr<Shader>);
    static void AddPostTreatment(const std::string& name, const std::string& path);
    static void RemovePostTreatment(std::shared_ptr<Shader>);
    static void StartRenderingThread();
    static void StopRenderingThread();
    static void RequestRedraw();
    static double DeltaTime() { return Instance()->_deltaTime; }
    static double FixedDeltaTime() { return Instance()->_fixedDeltaTime; }
    static std::atomic<bool>& NeedsUpdate();
    static std::atomic<bool>& Drawing();
    static uint32_t FrameNumber(void);
    static const std::shared_ptr<Geometry> DisplayQuad();
    static std::vector<std::shared_ptr<Shader>>& PostTreatments();
    static void SetInternalQuality(float);
    static float InternalQuality();
    static std::shared_ptr<Texture2D> BRDF();
    static void SetBRDF(std::shared_ptr<Texture2D>);

private:
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _UpdateCPU(float) override {};
    virtual void _FixedUpdateCPU(float) override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateGPU(float) override;
    virtual void _FixedUpdateGPU(float) override;
    static void _thread();
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
static auto lightingFragmentCode =
#include "lighting.frag"
    ;

static auto lightingEnvFragmentCode =
#include "lighting_env.frag"
    ;

static auto refractionFragmentCode =
#include "refraction.frag"
    ;

static auto SSRFragmentCode =
#include "SSR.frag"
    ;
static auto SSRMergeShaderCode =
#include "SSRMerge.frag"
    ;

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
    vao = Geometry::Create("DisplayQuad");
    vao->SetMode(GL_TRIANGLE_STRIP);
    vao->SetAccessor(Geometry::AccessorKey::Position, accessor);
    return vao;
}

std::shared_ptr<Framebuffer> CreateGeometryBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Framebuffer::Create(name, size, 0, 1);
    /*buffer->Create_attachement(GL_RGBA, GL_RGBA8); //BRDF CDiff
    buffer->Create_attachement(GL_RGBA, GL_RGBA8); //BRDF F0
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emissive;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); // MaterialValues -> Roughness, Metallic, Ior
    buffer->Create_attachement(GL_RED, GL_R8); //AO
    buffer->Create_attachement(GL_RGB, GL_RGB16_SNORM); // Normal;
    buffer->setup_attachements();*/

    buffer->Create_attachement(GL_RGBA, GL_RGBA8); // BRDF CDiff, Transparency;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emissive;
    buffer->Create_attachement(GL_RGBA, GL_RGBA8); // BRDF F0, BRDF Alpha;
    buffer->Create_attachement(GL_RED, GL_R16F); // Ior
    buffer->Create_attachement(GL_RED, GL_R8); //AO
    buffer->Create_attachement(GL_RG, GL_RG16_SNORM); // Normal;
    //buffer->setup_attachements();
    return (buffer);
}

std::shared_ptr<Framebuffer> CreateRenderBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Framebuffer::Create(name, size, 0, 1);
    buffer->Create_attachement(GL_RGBA, GL_RGBA8); // Color;
    buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emissive;
    //buffer->Create_attachement(GL_RGB, GL_RGB16_SNORM); //Normal
    //buffer->setup_attachements();
    return (buffer);
}

void present(std::shared_ptr<Framebuffer> back_buffer)
{
    static std::shared_ptr<Shader> presentShader;
    if (presentShader == nullptr) {
        presentShader = Shader::Create("present");
        presentShader->SetStage(ShaderStage::Create(GL_VERTEX_SHADER, ShaderCode::Create(passthrough_vertex_code, "PassThrough();")));
        presentShader->SetStage(ShaderStage::Create(GL_FRAGMENT_SHADER, ShaderCode::Create(present_fragment_code, "Present();")));
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

std::vector<std::shared_ptr<Light>> normalLights;
std::vector<std::shared_ptr<Light>> shadowLights;

//TODO Cleanup
void render_shadows()
{

    //auto camera = Scene::Current()->CurrentCamera();

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    /*glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);*/

    for (auto& light : shadowLights) {
        /*light->render_buffer()->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        tempCamera->SetPosition(light->Position());
        tempCamera->SetViewMatrix(light->TransformMatrix());
        Scene::Current()->RenderDepth(RenderMod::RenderOpaque);
        light->render_buffer()->bind(false);*/
        light->render_shadow();
    }
    //Scene::Current()->SetCurrentCamera(camera);
}

//double Render::Private::DeltaTime()
//{
//    return Instance()->_deltaTime;
//}

void Render::Private::SetInternalQuality(float q)
{
    Render::Private::Instance()->_internalQuality = q;
}

float Render::Private::InternalQuality()
{
    return Render::Private::Instance()->_internalQuality;
}

std::shared_ptr<Texture2D> Render::Private::BRDF()
{
    return Render::Private::Instance()->_brdf;
}

void Render::Private::SetBRDF(std::shared_ptr<Texture2D> brdf)
{
    Render::Private::Instance()->_brdf = brdf;
}

void Render::Private::RequestRedraw(void)
{
    Instance()->_needsUpdate = true;
}

void Render::Private::StartRenderingThread(void)
{
    Instance()->_loop = true;
    Instance()->_rendering_thread = std::thread(_thread);
}

void Render::Private::StopRenderingThread(void)
{
    Instance()->_loop = false;
    Instance()->_rendering_thread.join();
    SDL_GL_MakeCurrent(Window::sdl_window(), Window::context());
}

void Render::Private::_thread(void)
{
    double ticks;
    double lastTicks;
    double lastTicksFixed = lastTicks = SDL_GetTicks() / 1000.0;

    SDL_GL_MakeCurrent(Window::sdl_window(), Window::context());
    SDL_GL_SetSwapInterval(Engine::SwapInterval());
    std::shared_ptr<Texture2D> brdf = Texture2D::Create("brdf", glm::vec2(256, 256), GL_TEXTURE_2D, GL_RG, GL_RG8, GL_UNSIGNED_BYTE, brdfLUT);
    brdf->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    brdf->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    Render::Private::SetBRDF(brdf);
    while (Instance()->_loop) {
        if (Render::Private::NeedsUpdate()) {
            ticks = SDL_GetTicks() / 1000.0;
            Instance()->_frame_nbr++;
            Instance()->_deltaTime = ticks - lastTicks;
            lastTicks = ticks;
            Render::Private::Instance()->UpdateGPU(Instance()->_deltaTime);
            if (ticks - lastTicksFixed >= 0.015) {
                Instance()->_fixedDeltaTime = ticks - lastTicksFixed;
                lastTicksFixed = ticks;
                Render::Private::Instance()->FixedUpdateGPU(Instance()->_fixedDeltaTime);
            }
            Instance()->_drawing = true;
            Render::Private::Scene();
            Instance()->_drawing = false;
            Instance()->_needsUpdate = false;
        }
    }
    SDL_GL_MakeCurrent(Window::sdl_window(), nullptr);
}

uint32_t Render::Private::FrameNumber()
{
    return (Instance()->_frame_nbr);
}

std::shared_ptr<Render::Private> Render::Private::Create()
{
    return std::make_shared<Render::Private>();
}

std::shared_ptr<Render::Private> Render::Private::Instance()
{
    static auto instance = Render::Private::Create();
    return instance;
}

void Render::Private::_UpdateGPU(float delta)
{
    Scene::Current()->UpdateGPU(delta);
}

void Render::Private::_FixedUpdateGPU(float delta)
{
    //auto InvViewMatrix = glm::inverse(Scene::Current()->CurrentCamera()->ViewMatrix());
    //auto InvProjMatrix = glm::inverse(Scene::Current()->CurrentCamera()->ProjectionMatrix());
    //glm::ivec2 res = glm::vec2(Window::size()) * Render::Private::InternalQuality();
    //auto index = 0;

    shadowLights.reserve(1000);
    normalLights.reserve(1000);
    shadowLights.clear();
    normalLights.clear();
    Scene::Current()->FixedUpdateGPU(delta);
    for (auto light : Scene::Current()->Lights()) {
        if (light->power() == 0 || (!light->color().x && !light->color().y && !light->color().z))
            continue;
        if (light->cast_shadow())
            shadowLights.push_back(light);
        else
            normalLights.push_back(light);
    }
    render_shadows();
    /*index = 0;
    while (auto shader = Shader::Get(index)) {
        //shader->use();
        shader->SetUniform("Camera.Position", Scene::Current()->CurrentCamera()->GetComponent<Transform>()->WorldPosition());
        shader->SetUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
        shader->SetUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
        shader->SetUniform("Camera.InvMatrix.View", InvViewMatrix);
        shader->SetUniform("Camera.InvMatrix.Projection", InvProjMatrix);
        shader->SetUniform("Resolution", glm::vec3(res.x, res.y, res.x / res.y));
        shader->SetUniform("Time", SDL_GetTicks() / 1000.f);
        //shader->use(false);
        index++;
    }*/
}

std::shared_ptr<Framebuffer> light_pass(std::shared_ptr<Framebuffer>& currentGeometryBuffer)
{
    auto lightsPerPass = Config::Get("LightsPerPass", 8u);
    auto shadowsPerPass = Config::Get("ShadowsPerPass", 8u);
    static std::shared_ptr<Framebuffer> lightRenderBuffer0 = CreateRenderBuffer("lightRenderBuffer0", glm::vec2(currentGeometryBuffer->Size()));
    if (lightsPerPass == 0)
        return lightRenderBuffer0;
    static std::shared_ptr<Framebuffer> lightRenderBuffer1 = CreateRenderBuffer("lightRenderBuffer1", glm::vec2(currentGeometryBuffer->Size()));
    lightRenderBuffer0->Resize(glm::vec2(currentGeometryBuffer->Size()));
    lightRenderBuffer1->Resize(glm::vec2(currentGeometryBuffer->Size()));
    static auto lighting_shader = Shader::Create("lighting", LightingShader);
    lighting_shader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(lightingFragmentCode, "Lighting();"));
    lighting_shader->SetDefine("LIGHTNBR", std::to_string(lightsPerPass));
    lighting_shader->SetDefine("PointLight", std::to_string(Point));
    lighting_shader->SetDefine("DirectionnalLight", std::to_string(Directionnal));
    static auto slighting_shader = Shader::Create("shadow_lighting", LightingShader);
    slighting_shader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(lightingFragmentCode, "Lighting();"));
    slighting_shader->SetDefine(("SHADOWNBR"), std::to_string(shadowsPerPass));
    slighting_shader->SetDefine(("LIGHTNBR"), std::to_string(lightsPerPass));
    slighting_shader->SetDefine(("PointLight"), std::to_string(Point));
    slighting_shader->SetDefine(("DirectionnalLight"), std::to_string(Directionnal));
    if (shadowsPerPass > 0)
        slighting_shader->SetDefine("SHADOW");
    auto actualShadowNbr = std::max(1u, shadowsPerPass);
    auto shader = lighting_shader;
    lightRenderBuffer1->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    lightRenderBuffer1->bind(false);
    for (auto i = 0u, j = 0u; i < normalLights.size() || j < shadowLights.size();) {
        if (normalLights.size() - i < lightsPerPass && shadowLights.size() - j != 0)
            shader = slighting_shader;
        else
            shader = lighting_shader;
        auto lightIndex = 0u;
        while (lightIndex < lightsPerPass && i < normalLights.size()) {
            auto light = normalLights.at(i);
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].Position", light->GetComponent<Transform>()->WorldPosition());
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].Color", light->color() * light->power());
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].Type", int(light->type()));
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].ShadowIndex", -1);
            i++;
            lightIndex++;
        }
        auto shadowIndex = 0u;
        while (lightIndex < lightsPerPass && shadowIndex < actualShadowNbr && j < shadowLights.size()) {
            auto light = shadowLights.at(j);
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].Position", light->GetComponent<Transform>()->WorldPosition());
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].Color", light->color() * light->power());
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].Type", int(light->type()));
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].ShadowIndex", int(shadowIndex));
            shader->SetUniform("Light[" + std::to_string(lightIndex) + "].Projection", light->ShadowProjectionMatrix());
            shader->SetTexture("Shadow[" + std::to_string(shadowIndex) + "]", light->render_buffer()->depth());
            j++;
            lightIndex++;
            shadowIndex++;
        }
        if (lightIndex == 0)
            continue;
        shader->SetTexture("Texture.Geometry.CDiff", currentGeometryBuffer->attachement(0));
        shader->SetTexture("Texture.Geometry.Emissive", currentGeometryBuffer->attachement(1));
        shader->SetTexture("Texture.Geometry.F0", currentGeometryBuffer->attachement(2));
        shader->SetTexture("Texture.Geometry.Ior", currentGeometryBuffer->attachement(3));
        shader->SetTexture("Texture.Geometry.Normal", currentGeometryBuffer->attachement(5));
        shader->SetTexture("Texture.Geometry.Depth", currentGeometryBuffer->depth());
        shader->SetTexture("Texture.Back.Color", lightRenderBuffer1->attachement(0));
        shader->SetTexture("Texture.Back.Emissive", lightRenderBuffer1->attachement(1));
        //shader->SetUniform("Texture.Back.Normal", lightRenderBuffer1->attachement(2), GL_TEXTURE8);
        lightRenderBuffer0->bind();
        shader->use();
        Render::Private::DisplayQuad()->Draw();
        shader->use(false);
        std::swap(lightRenderBuffer0, lightRenderBuffer1);
    }
    return lightRenderBuffer1;
}

std::atomic<bool>& Render::Private::NeedsUpdate()
{
    return (Instance()->_needsUpdate);
}

std::atomic<bool>& Render::Private::Drawing()
{
    return (Instance()->_drawing);
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
        auto ShaderVertexCode = ShaderCode::Create();
        ShaderVertexCode->SetCode(PassThroughVertexCode);
        ShaderVertexCode->SetTechnique("PassThrough();");
        auto ShaderFragmentCode = ShaderCode::Create();
        ShaderFragmentCode->SetCode(HZBFragmentCode);
        ShaderFragmentCode->SetTechnique("HZB();");
        HZBShader = Shader::Create("HZB");
        HZBShader->SetStage(ShaderStage::Create(GL_VERTEX_SHADER, ShaderVertexCode));
        HZBShader->SetStage(ShaderStage::Create(GL_FRAGMENT_SHADER, ShaderFragmentCode));
    }
    static auto framebuffer = Framebuffer::Create("HZB", depthTexture->Size(), 0, 0);
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
    glm::ivec2 res = glm::vec2(1024) * Render::Private::InternalQuality() * Config::Get("SSAOResolutionFactor", 0.5f);
    glDepthMask(false);
    static auto SSAOFragmentCode =
#include "ssao.frag"
        ;
    static auto SSAOApplyFragmentCode =
#include "ssaoApply.frag"
        ;
    static auto SSAOResult(Framebuffer::Create("SSAOBuffer", res, 1, 0));
    static std::shared_ptr<Shader> SSAOShader;
    static std::shared_ptr<Shader> SSAOApplyShader;
    if (SSAOShader == nullptr) {
        SSAOShader = Shader::Create("SSAO", LightingShader);
        SSAOShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(SSAOFragmentCode, "SSAO();"));
        Render::Private::Instance()->AddComponent(SSAOShader);
        //SSAOShader->SetStage(ShaderStage::Create(GL_VERTEX_SHADER));
        //SSAOShader->SetStage(ShaderStage::Create(GL_FRAGMENT_SHADER, ShaderCode::Create(SSAOFragmentCode, "SSAO();")));
    }
    if (SSAOApplyShader == nullptr) {
        SSAOApplyShader = Shader::Create("SSAOApply");
        SSAOApplyShader->SetStage(ShaderStage::Create(GL_VERTEX_SHADER, ShaderCode::Create(passthrough_vertex_code, "PassThrough();")));
        SSAOApplyShader->SetStage(ShaderStage::Create(GL_FRAGMENT_SHADER, ShaderCode::Create(SSAOApplyFragmentCode, "SSAOApply();")));
        Render::Private::Instance()->AddComponent(SSAOApplyShader);
    }
    SSAOShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SSAO_QUALITY", std::to_string(Config::Get("SSAOQuality", 4)));
    SSAOShader->SetTexture("Texture.Geometry.AO", gBuffer->attachement(4));
    SSAOShader->SetTexture("Texture.Geometry.Normal", gBuffer->attachement(5));
    SSAOShader->SetTexture("Texture.Geometry.Depth", gBuffer->depth());
    SSAOResult->Resize(res);
    SSAOResult->bind();
    glClear(Window::clear_mask());
    SSAOShader->use();
    Render::DisplayQuad()->Draw();
    SSAOShader->use(false);
    SSAOResult->bind(false);

    SSAOResult->attachement(0)->blur(1, 1);

    SSAOApplyShader->SetTexture("Texture.Geometry.AO", SSAOResult->attachement(0));
    gBuffer->bind();
    SSAOApplyShader->use();
    Render::DisplayQuad()->Draw();
    SSAOApplyShader->use(false);
    gBuffer->bind(false);
    glDepthMask(true);
}

float ComputeRoughnessMaskScale()
{
    float MaxRoughness = std::clamp(Config::Get("SSRMaxRoughness", 0.8f), 0.01f, 1.0f);

    float RoughnessMaskScale = -2.0f / MaxRoughness;
    return RoughnessMaskScale * (Config::Get("SSRQuality", 4) < 3 ? 2.0f : 1.0f);
}

static std::shared_ptr<Shader> SSRShader;
static std::shared_ptr<Shader> SSRMergeShader;

std::shared_ptr<Texture2D> SSRPass1(std::shared_ptr<Framebuffer> gBuffer, std::shared_ptr<Framebuffer> lastRender)
{
    glm::ivec2 res = glm::vec2(1024) * Config::Get("SSRResolutionFactor", 1.f);
    static auto SSRFramebuffer0(Framebuffer::Create("SSRFramebuffer0", res, 1, 0));
    static auto SSRFramebuffer1(Framebuffer::Create("SSRFramebuffer1", res, 1, 0));
    static auto SSRFramebufferResult(Framebuffer::Create("SSRFramebufferResult", res, 1, 0));
    if (SSRShader == nullptr) {
        SSRShader = Shader::Create("SSR1", LightingShader);
        SSRShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(SSRFragmentCode, "SSR();"));
        Render::Private::Instance()->AddComponent(SSRShader);
    }
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SSR_QUALITY", std::to_string(Config::Get("SSRQuality", 4)));
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SCREEN_BORDER_FACTOR", std::to_string(Config::Get("SSRBorderFactor", 10)));
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("ROUGHNESSMASKSCALE", std::to_string(ComputeRoughnessMaskScale()));
    if (SSRMergeShader == nullptr) {
        SSRMergeShader = Shader::Create("SSRMerge", LightingShader);
        SSRMergeShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(SSRMergeShaderCode, "SSRMerge();"));
        Render::Private::Instance()->AddComponent(SSRMergeShader);
    }
    static std::shared_ptr<Framebuffer> currentFrameBuffer = nullptr;
    SSRFramebuffer0->Resize(res);
    SSRFramebuffer1->Resize(res);
    SSRFramebufferResult->Resize(res);
    currentFrameBuffer = currentFrameBuffer == SSRFramebuffer0 ? SSRFramebuffer1 : SSRFramebuffer0;
    auto lastFrameBuffer = currentFrameBuffer == SSRFramebuffer0 ? SSRFramebuffer1 : SSRFramebuffer0;

    HZBPass(gBuffer->depth());
    SSRShader->SetTexture("Texture.Geometry.Normal", gBuffer->attachement(5));
    SSRShader->SetTexture("Texture.Geometry.Depth", gBuffer->depth());
    SSRShader->SetTexture("Texture.Geometry.F0", gBuffer->attachement(2));
    SSRShader->SetTexture("LastColor", lastRender->attachement(0));
    SSRShader->SetUniform("FrameBufferResolution", res);
    SSRShader->SetUniform("FrameNumber", Render::FrameNumber());
    currentFrameBuffer->Resize(res);
    currentFrameBuffer->bind();
    SSRShader->use();
    Render::Private::DisplayQuad()->Draw();
    SSRShader->use(false);

    //Merge the current result with last result to increase sampling
    SSRMergeShader->SetTexture("Texture.Geometry.Depth", gBuffer->depth());
    SSRMergeShader->SetTexture("in_Texture_Color", currentFrameBuffer->attachement(0));
    SSRMergeShader->SetTexture("in_Last_Texture_Color", lastFrameBuffer->attachement(0));
    SSRFramebufferResult->bind();
    SSRMergeShader->use();
    Render::DisplayQuad()->Draw();
    SSRMergeShader->use(false);
    SSRFramebufferResult->bind(false);

    SSRShader->SetUniform("LastViewMatrix", Scene::Current()->CurrentCamera()->ViewMatrix());
    SSRShader->SetUniform("LastProjectionMatrix", Scene::Current()->CurrentCamera()->ProjectionMatrix());
    SSRMergeShader->SetUniform("LastViewMatrix", Scene::Current()->CurrentCamera()->ViewMatrix());
    SSRMergeShader->SetUniform("LastProjectionMatrix", Scene::Current()->CurrentCamera()->ProjectionMatrix());

    return SSRFramebufferResult->attachement(0);
}
/*
struct SSR {
    static auto Resolution()
    {
        return glm::vec2(1024) * Config::Get("SSRResolutionFactor", 1.f);
    }
    static auto Apply(std::shared_ptr<Framebuffer> gBuffer, std::shared_ptr<Framebuffer> lastRender)
    {
        static auto SSRFramebuffer(Framebuffer::Create("SSRFramebuffer", Resolution(), 1, 0));
        static auto SSRFramebufferResult(Framebuffer::Create("SSRFramebufferResult", Resolution(), 1, 0));
        static std::shared_ptr<Shader> SSRShader;
        static std::shared_ptr<Shader> SSRBlurShader;
        if (SSRShader == nullptr) {
            SSRShader = Shader::Create("SSR0", LightingShader);
            SSRShader->Stage(GL_FRAGMENT_SHADER)->SetTechnique(SSRShaderCode);
        }
        if (SSRBlurShader == nullptr) {
            SSRBlurShader = Shader::Create("SSRBlur", LightingShader);
            SSRBlurShader->Stage(GL_FRAGMENT_SHADER)->SetTechnique(SSRBlurShaderCode);
        }
        SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SSR_QUALITY", std::to_string(Config::Get("SSRQuality", 4)));
        SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SCREEN_BORDER_FACTOR", std::to_string(Config::Get("SSRBorderFactor", 10)));
        SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("ROUGHNESSMASKSCALE", std::to_string(ComputeRoughnessMaskScale()));
        SSRFramebuffer->Resize(Resolution());

        HZBPass(gBuffer->depth());

        SSRShader->SetUniform("Texture.Normal", gBuffer->attachement(5), GL_TEXTURE0);
        SSRShader->SetUniform("Texture.Depth", gBuffer->depth(), GL_TEXTURE1);
        SSRShader->SetUniform("Texture.MaterialValues", gBuffer->attachement(3), GL_TEXTURE2);
        SSRShader->SetUniform("LastColor", lastRender->attachement(0), GL_TEXTURE3);
        SSRShader->SetUniform("FrameBufferResolution", Resolution());
        SSRShader->SetUniform("FrameNumber", Render::FrameNumber());
        SSRFramebuffer->Resize(Resolution());
        SSRFramebuffer->bind();
        SSRShader->use();
        Render::Private::DisplayQuad()->Draw();
        SSRShader->use(false);

        //Blur the result
        SSRBlurShader->SetUniform("Texture.MaterialValues", gBuffer->attachement(3), GL_TEXTURE1);
        SSRFramebuffer->attachement(0)->blur(1, 1, SSRBlurShader);

        SSRShader->SetUniform("LastViewMatrix", Scene::Current()->CurrentCamera()->ViewMatrix());
        SSRShader->SetUniform("LastProjectionMatrix", Scene::Current()->CurrentCamera()->ProjectionMatrix());
        return SSRFramebufferResult->attachement(0);
    };
};
*/

std::shared_ptr<Texture2D>
SSRPass0(std::shared_ptr<Framebuffer> gBuffer, std::shared_ptr<Framebuffer> lastRender)
{
    glm::ivec2 res = glm::vec2(1024) * Config::Get("SSRResolutionFactor", 1.f);
    static auto SSRFramebuffer0(Framebuffer::Create("SSRFramebuffer0", res, 1, 0));
    static auto SSRFramebuffer1(Framebuffer::Create("SSRFramebuffer1", res, 1, 0));
    static auto SSRFramebufferResult(Framebuffer::Create("SSRFramebufferResult", res, 1, 0));
    static std::shared_ptr<Shader> SSRShader;
    static std::shared_ptr<Shader> SSRMergeShader;
    if (SSRShader == nullptr) {
        SSRShader = Shader::Create("SSR0", LightingShader);
        SSRShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(SSRFragmentCode, "SSR();"));
        Render::Private::Instance()->AddComponent(SSRShader);
    }
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SSR_QUALITY", std::to_string(Config::Get("SSRQuality", 4)));
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SCREEN_BORDER_FACTOR", std::to_string(Config::Get("SSRBorderFactor", 10)));
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("ROUGHNESSMASKSCALE", std::to_string(ComputeRoughnessMaskScale()));
    if (SSRMergeShader == nullptr) {
        SSRMergeShader = Shader::Create("SSRMerge", LightingShader);
        SSRMergeShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(SSRMergeShaderCode, "SSRMerge();"));
        Render::Private::Instance()->AddComponent(SSRMergeShader);
    }
    static std::shared_ptr<Framebuffer> currentFrameBuffer = nullptr;
    SSRFramebuffer0->Resize(res);
    SSRFramebuffer1->Resize(res);
    SSRFramebufferResult->Resize(res);
    currentFrameBuffer = currentFrameBuffer == SSRFramebuffer0 ? SSRFramebuffer1 : SSRFramebuffer0;
    auto lastFrameBuffer = currentFrameBuffer == SSRFramebuffer0 ? SSRFramebuffer1 : SSRFramebuffer0;

    HZBPass(gBuffer->depth());
    SSRShader->SetTexture("Texture.Geometry.Normal", gBuffer->attachement(5));
    SSRShader->SetTexture("Texture.Geometry.Depth", gBuffer->depth());
    SSRShader->SetTexture("Texture.Geometry.F0", gBuffer->attachement(2));
    SSRShader->SetTexture("LastColor", lastRender->attachement(0));
    SSRShader->SetUniform("FrameBufferResolution", res);
    SSRShader->SetUniform("FrameNumber", Render::FrameNumber());
    currentFrameBuffer->Resize(res);
    currentFrameBuffer->bind();
    SSRShader->use();
    Render::Private::DisplayQuad()->Draw();
    SSRShader->use(false);

    //Merge the current result with last result to increase sampling
    SSRMergeShader->SetTexture("Texture.Geometry.Depth", gBuffer->depth());
    SSRMergeShader->SetTexture("in_Texture_Color", currentFrameBuffer->attachement(0));
    SSRMergeShader->SetTexture("in_Last_Texture_Color", lastFrameBuffer->attachement(0));
    SSRFramebufferResult->bind();
    SSRMergeShader->use();
    Render::DisplayQuad()->Draw();
    SSRMergeShader->use(false);
    SSRFramebufferResult->bind(false);

    SSRShader->SetUniform("LastViewMatrix", Scene::Current()->CurrentCamera()->ViewMatrix());
    SSRShader->SetUniform("LastProjectionMatrix", Scene::Current()->CurrentCamera()->ProjectionMatrix());
    SSRMergeShader->SetUniform("LastViewMatrix", Scene::Current()->CurrentCamera()->ViewMatrix());
    SSRMergeShader->SetUniform("LastProjectionMatrix", Scene::Current()->CurrentCamera()->ProjectionMatrix());

    return SSRFramebufferResult->attachement(0);
}

std::shared_ptr<Framebuffer> RefractionPass(std::shared_ptr<Framebuffer> geometryBuffer, std::shared_ptr<Framebuffer> opaqueRenderBuffer)
{
    glm::ivec2 res = geometryBuffer->Size();
    static auto refractionRenderBuffer(CreateRenderBuffer("refractionRenderBuffer", res));
    static std::shared_ptr<Shader> refractionShader;
    if (refractionShader == nullptr) {
        refractionShader = Shader::Create("refraction", LightingShader);
        refractionShader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(refractionFragmentCode, "Refraction();"));
        Render::Private::Instance()->AddComponent(refractionShader);
    }
    auto i = 0;
    refractionShader->SetTexture("Texture.Geometry.CDiff", geometryBuffer->attachement(0));
    refractionShader->SetTexture("Texture.Geometry.Emissive", geometryBuffer->attachement(1));
    refractionShader->SetTexture("Texture.Geometry.F0", geometryBuffer->attachement(2));
    refractionShader->SetTexture("Texture.Geometry.Ior", geometryBuffer->attachement(3));
    refractionShader->SetTexture("Texture.Geometry.AO", geometryBuffer->attachement(4));
    refractionShader->SetTexture("Texture.Geometry.Normal", geometryBuffer->attachement(5));
    refractionShader->SetTexture("Texture.Geometry.Depth", geometryBuffer->depth());
    refractionShader->SetTexture("Texture.BRDF", Render::Private::BRDF());
    refractionShader->SetTexture("Texture.Back.Color", opaqueRenderBuffer->attachement(0));
    refractionShader->SetTexture("Texture.Back.Emissive", opaqueRenderBuffer->attachement(1));
    //refractionShader->SetUniform("Texture.Back.Normal", opaqueRenderBuffer->attachement(2));
    refractionRenderBuffer->Resize(res);
    refractionRenderBuffer->bind();
    refractionShader->use();
    Render::Private::DisplayQuad()->Draw();
    refractionShader->use(false);
    refractionRenderBuffer->bind(false);
    return refractionRenderBuffer;
}

std::shared_ptr<Framebuffer> OpaquePass(std::shared_ptr<Framebuffer> lastRender)
{
    glm::ivec2 res = glm::vec2(Window::size()) * Render::Private::InternalQuality();
    static auto opaqueGeometryBuffer(CreateGeometryBuffer("opaqueGeometryBuffer", res));
    static auto opaqueRenderBuffer(CreateRenderBuffer("opaqueRenderBuffer", res));
    static std::shared_ptr<Shader> elighting_shader;
    if (elighting_shader == nullptr) {
        elighting_shader = Shader::Create("opaque_lighting_env", LightingShader);
        elighting_shader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(lightingEnvFragmentCode, "Lighting();"));
        Render::Private::Instance()->AddComponent(elighting_shader);
    }
    opaqueGeometryBuffer->Resize(res);
    opaqueRenderBuffer->Resize(res);
    opaqueGeometryBuffer->bind();
    glClear(Window::clear_mask());
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Scene::Current()->Render(RenderMod::RenderOpaque);
    opaqueGeometryBuffer->bind(false);

    auto SSRResult(SSRPass0(opaqueGeometryBuffer, lastRender));
    auto lightResult(light_pass(opaqueGeometryBuffer));

    SSAOPass(opaqueGeometryBuffer);

    auto i = 0;
    elighting_shader->SetTexture("Texture.Geometry.CDiff", opaqueGeometryBuffer->attachement(0));
    elighting_shader->SetTexture("Texture.Geometry.Emissive", opaqueGeometryBuffer->attachement(1));
    elighting_shader->SetTexture("Texture.Geometry.F0", opaqueGeometryBuffer->attachement(2));
    elighting_shader->SetTexture("Texture.Geometry.Ior", opaqueGeometryBuffer->attachement(3));
    elighting_shader->SetTexture("Texture.Geometry.AO", opaqueGeometryBuffer->attachement(4));
    elighting_shader->SetTexture("Texture.Geometry.Normal", opaqueGeometryBuffer->attachement(5));
    elighting_shader->SetTexture("Texture.Geometry.Depth", opaqueGeometryBuffer->depth());
    elighting_shader->SetTexture("Texture.BRDF", Render::Private::BRDF());
    elighting_shader->SetTexture("Texture.Back.Color", lightResult->attachement(0));
    elighting_shader->SetTexture("Texture.Back.Emissive", lightResult->attachement(1));
    elighting_shader->SetTexture("SSRResult", SSRResult);
    if (Environment::current() != nullptr) {
        elighting_shader->SetTexture("Texture.Environment.Diffuse", Environment::current()->diffuse());
        elighting_shader->SetTexture("Texture.Environment.Irradiance", Environment::current()->irradiance());
    }
    opaqueRenderBuffer->bind();
    elighting_shader->use();
    Render::Private::DisplayQuad()->Draw();
    elighting_shader->use(false);
    opaqueRenderBuffer->bind(false);
    return opaqueRenderBuffer;
}

std::shared_ptr<Framebuffer> TranspPass(std::shared_ptr<Framebuffer> lastRender, std::shared_ptr<Framebuffer> opaqueRenderBuffer)
{
    glm::ivec2 res = opaqueRenderBuffer->Size();
    static auto transpGeometryBuffer(CreateGeometryBuffer("transpGeometryBuffer", res));
    static auto transpRenderBuffer(CreateRenderBuffer("transpRenderBuffer", res));
    static std::shared_ptr<Shader> elighting_shader;
    if (elighting_shader == nullptr) {
        elighting_shader = Shader::Create("lighting_env_transparent", LightingShader);
        elighting_shader->Stage(GL_FRAGMENT_SHADER)->AddExtension(ShaderCode::Create(lightingEnvFragmentCode, "Lighting();"));
        elighting_shader->SetDefine("TRANSPARENT");
        Render::Private::Instance()->AddComponent(elighting_shader);
    }
    transpGeometryBuffer->SetDepthBuffer(opaqueRenderBuffer->depth());
    transpRenderBuffer->Resize(res);
    transpGeometryBuffer->Resize(res);

    //Transparent pass
    transpGeometryBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Scene::Current()->Render(RenderMod::RenderTransparent);
    transpGeometryBuffer->bind(false);

    //SSAOPass(transpGeometryBuffer);

    auto SSRResult(SSRPass1(transpGeometryBuffer, lastRender));

    auto refractionResult(RefractionPass(transpGeometryBuffer, opaqueRenderBuffer));

    auto i = 0;
    elighting_shader->SetTexture("Texture.Geometry.CDiff", transpGeometryBuffer->attachement(0));
    elighting_shader->SetTexture("Texture.Geometry.Emissive", transpGeometryBuffer->attachement(1));
    elighting_shader->SetTexture("Texture.Geometry.F0", transpGeometryBuffer->attachement(2));
    elighting_shader->SetTexture("Texture.Geometry.Ior", transpGeometryBuffer->attachement(3));
    elighting_shader->SetTexture("Texture.Geometry.AO", transpGeometryBuffer->attachement(4));
    elighting_shader->SetTexture("Texture.Geometry.Normal", transpGeometryBuffer->attachement(5));
    elighting_shader->SetTexture("Texture.Geometry.Depth", transpGeometryBuffer->depth());
    elighting_shader->SetTexture("Texture.BRDF", Render::Private::BRDF());
    elighting_shader->SetTexture("Texture.Back.Color", refractionResult->attachement(0));
    elighting_shader->SetTexture("Texture.Back.Emissive", refractionResult->attachement(1));
    elighting_shader->SetTexture("SSRResult", SSRResult);
    if (Environment::current() != nullptr) {
        elighting_shader->SetTexture("Texture.Environment.Diffuse", Environment::current()->diffuse());
        elighting_shader->SetTexture("Texture.Environment.Irradiance", Environment::current()->irradiance());
    }
    transpRenderBuffer->bind();
    elighting_shader->use();
    Render::Private::DisplayQuad()->Draw();
    elighting_shader->use(false);
    transpRenderBuffer->bind(false);
    return transpRenderBuffer;
}

void Render::Private::Scene()
{
    if (!Render::Private::NeedsUpdate() || Scene::Current() == nullptr || Scene::Current()->CurrentCamera() == nullptr) {
        //present(final_back_buffer);
        return;
    }
    glm::ivec2 res = glm::vec2(Window::size()) * Render::Private::InternalQuality();
    //Setup framebuffers
    static auto finalRenderBuffer(CreateRenderBuffer("finalRenderBuffer", res));
    static auto lastRender(CreateRenderBuffer("lastRender", res / 8));
    finalRenderBuffer->Resize(res);
    lastRender->Resize(res / 8);

    //Setup shaders

    static std::shared_ptr<Shader> passthrough_shader;
    if (passthrough_shader == nullptr) {
        passthrough_shader = Shader::Create("passthrough");
        passthrough_shader->SetStage(ShaderStage::Create(GL_VERTEX_SHADER, ShaderCode::Create(passthrough_vertex_code, "PassThrough();")));
        passthrough_shader->SetStage(ShaderStage::Create(GL_FRAGMENT_SHADER, ShaderCode::Create(passthrough_fragment_code, "PassThrough();")));
        Render::Private::Instance()->AddComponent(passthrough_shader);
    }

    auto opaqueRenderBuffer(OpaquePass(lastRender));
    opaqueRenderBuffer->attachement(0)->generate_mipmap();
    opaqueRenderBuffer->attachement(0)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    opaqueRenderBuffer->attachement(1)->generate_mipmap();
    opaqueRenderBuffer->attachement(1)->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    auto transpRenderBuffer(TranspPass(lastRender, opaqueRenderBuffer));
    transpRenderBuffer->attachement(1)->blur(Config::Get("BloomPass", 1), 3.5);

    lastRender->bind();
    glClear(Window::clear_mask());
    lastRender->bind();
    passthrough_shader->SetTexture("in_Buffer0", transpRenderBuffer->attachement(0));
    passthrough_shader->SetTexture("in_Buffer1", transpRenderBuffer->attachement(1));
    passthrough_shader->SetTexture("in_Texture_Depth", transpRenderBuffer->depth());
    passthrough_shader->use();
    Render::Private::DisplayQuad()->Draw();
    passthrough_shader->use(false);
    lastRender->bind(false);

    present(transpRenderBuffer);
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

std::atomic<bool>& Render::NeedsUpdate()
{
    return Render::Private::NeedsUpdate();
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

std::atomic<bool>& Render::Drawing()
{
    return Render::Private::Drawing();
}

const std::shared_ptr<Geometry> Render::DisplayQuad()
{
    return Render::Private::DisplayQuad();
}

uint32_t Render::FrameNumber()
{
    return Render::Private::FrameNumber();
}