/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:08
*/

#include "Render.hpp"
#include "Camera/Camera.hpp" // for Camera
#include "Config.hpp" // for Config
#include "Engine.hpp" // for UpdateMutex, SwapInterval
#include "Environment.hpp" // for Environment
#include "Framebuffer.hpp" // for Framebuffer
#include "Light/Light.hpp" // for Light, Directionnal, Point
#include "Mesh/Geometry.hpp" // for Geometry
#include "Scene/Scene.hpp"
#include "Shader/Shader.hpp" // for Shader
#include "Texture/Cubemap.hpp"
#include "Texture/Texture2D.hpp" // for Texture2D
#include "Window.hpp" // for Window
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
struct Private : public Component {
    static Render::Private& Instance();
    static void Scene();
    static void AddPostTreatment(std::shared_ptr<Shader>);
    static void AddPostTreatment(const std::string& name, const std::string& path);
    static void RemovePostTreatment(std::shared_ptr<Shader>);
    static std::vector<std::shared_ptr<Shader>>& PostTreatments();
    virtual std::shared_ptr<Component> _Clone() override
    {
        return nullptr;
        //return Component::Create<Private>(*this);
    }

    std::shared_ptr<Framebuffer> _opaqueBuffer;
    std::shared_ptr<Framebuffer> _lightBuffer;
    std::shared_ptr<Framebuffer> _geometryBuffer;
    uint32_t _frameNbr{ 0 };
    bool _loop { true };
    std::atomic<bool> _needsUpdate { true };
    std::atomic<bool> _drawing { false };
    std::thread _rendering_thread;
    std::shared_ptr<Texture2D> _brdf;
    Signal<float> _onAfterRender;
    Signal<float> _onBeforeRender;
    Render::Context _context{ nullptr };
};
} // namespace Render

// quad is a singleton
const std::shared_ptr<Geometry> Render::DisplayQuad()
{
    static std::shared_ptr<Geometry> vao;
    if (vao != nullptr) {
        return vao;
    }
    auto accessor(Component::Create<BufferAccessor>(BufferAccessor::ComponentType::Int8, BufferAccessor::Type::Vec2, 3));
    vao = Component::Create<Geometry>("DisplayQuad");
    vao->SetDrawingMode(Geometry::DrawingMode::Triangles);
    vao->SetAccessor(Geometry::AccessorKey::Position, accessor);
    return vao;
}

void Render::Private::RemovePostTreatment(std::shared_ptr<Shader>)
{
    //TODO : re-implement this
}


void PrintExtensions()
{
    debugLog("GL Extensions :");
    GLint n;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for (auto i = 0; i < n; ++i) {
        debugLog(glGetStringi(GL_EXTENSIONS, i));
    }
}

Render::Private& Render::Private::Instance()
{
    static std::shared_ptr<Render::Private> instance;
    if (instance == nullptr)
    {
        instance = Component::Create<Render::Private>();
    }
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
    static auto framebuffer = Component::Create<Framebuffer>("HZB", depthTexture->GetSize(), 0, 0);
    depthTexture->GenerateMipmap();
    depthTexture->SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::LinearMipmapLinear);
    auto numLevels = MIPMAPNBR(depthTexture->GetSize()); //1 + unsigned(floorf(log2f(fmaxf(depthTexture->GetSize().x, depthTexture->GetSize().y))));
    auto currentSize = depthTexture->GetSize();
    for (auto i = 1; i < numLevels; i++) {
        depthTexture->SetParameter<Texture::Parameter::BaseLevel>(i - 1);
        depthTexture->SetParameter<Texture::Parameter::MaxLevel>(i - 1);
        currentSize /= 2;
        currentSize = glm::max(currentSize, glm::ivec2(1));
        framebuffer->Resize(currentSize);
        //framebuffer->set_attachement(0, depthTexture, i);
        framebuffer->SetDepthBuffer(depthTexture, i);
        framebuffer->bind();
        HZBShader->SetTexture("in_Texture_Color", depthTexture);
        HZBShader->use();
        Render::DisplayQuad()->Draw();
        HZBShader->use(false);
        framebuffer->SetDepthBuffer(nullptr);
        //framebuffer->set_attachement(0, nullptr);
    }
    depthTexture->SetParameter<Texture::Parameter::BaseLevel>(0);
    depthTexture->SetParameter<Texture::Parameter::MaxLevel>(numLevels - 1);
    depthTexture->SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::LinearMipmapLinear);
    framebuffer->bind(false);
}

void SSAOPass(std::shared_ptr<Framebuffer> gBuffer)
{
    glm::ivec2 res = gBuffer->Size(); // glm::vec2(1024) * Render::Private::InternalQuality();// *Config::Global().Get("SSAOResolutionFactor", 0.5f);
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
    SSAOShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SSAO_QUALITY", std::to_string(Config::Global().Get("SSAOQuality", 4)));
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
    float MaxRoughness = std::clamp(Config::Global().Get("SSRMaxRoughness", 0.8f), 0.01f, 1.0f);

    float RoughnessMaskScale = -2.0f / MaxRoughness;
    return RoughnessMaskScale * (Config::Global().Get("SSRQuality", 4) < 3 ? 2.0f : 1.0f);
}

struct RenderHistory {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    std::shared_ptr<Framebuffer> framebuffer;
};

auto SSRPass(std::shared_ptr<Framebuffer> gBuffer, const RenderHistory& lastRender, std::shared_ptr<Framebuffer> lightBuffer)
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
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SSR_QUALITY", std::to_string(Config::Global().Get("SSRQuality", 4)));
    SSRShader->Stage(GL_FRAGMENT_SHADER)->SetDefine("SCREEN_BORDER_FACTOR", std::to_string(Config::Global().Get("SSRBorderFactor", 10)));
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
    Render::DisplayQuad()->Draw();
    SSRShader->use(false);
    lightBuffer->bind(false);
    glDisable(GL_BLEND);

    //lightBuffer->attachement(1)->blur(1, 1.5);
}

std::shared_ptr<Framebuffer> CreateGeometryBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Component::Create<Framebuffer>(name, size, 0, 1);

    buffer->Create_attachement(Pixel::SizedFormat::Uint8_NormalizedRGBA); // BRDF CDiff, Transparency;
    buffer->Create_attachement(Pixel::SizedFormat::Float16_RGB); // Emissive;
    buffer->Create_attachement(Pixel::SizedFormat::Uint8_NormalizedRGBA); // BRDF F0, BRDF Alpha;
    buffer->Create_attachement(Pixel::SizedFormat::Uint8_NormalizedR); //AO
    buffer->Create_attachement(Pixel::SizedFormat::Int8_NormalizedRGB); // Normal;
    auto idTexture = buffer->Create_attachement(Pixel::SizedFormat::Uint32_R);
    idTexture->SetParameter<Texture::Parameter::MagFilter>(Texture::Filter::Nearest);
    idTexture->SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::Nearest);
    buffer->Create_attachement(Pixel::SizedFormat::Float16_RG); //Velocity
    return (buffer);
}

auto CreateHistoryBuffer(glm::ivec2 res)
{
    auto buffer = Component::Create<Framebuffer>("HistoryBuffer", res, 1, 0);
    //buffer->Create_attachement(GL_RGB, GL_RGB8); //Color
    //buffer->Create_attachement(GL_RGB, GL_R11F_G11F_B10F); //Emissive
    return buffer;
}

std::shared_ptr<Framebuffer> CreateLightingBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Component::Create<Framebuffer>(name, size, 0, 0);
    buffer->Create_attachement(Pixel::SizedFormat::Float16_RGBA); // Diffuse, Specular Luminance;
    //buffer->Create_attachement(GL_RGB, GL_RGB16F); // Specular;
    buffer->Create_attachement(Pixel::SizedFormat::Uint8_NormalizedRGBA); // Reflection;
    return (buffer);
}

auto CreateOpaqueMaterialBuffer(glm::ivec2 res)
{
    auto buffer = Component::Create<Framebuffer>("OpaqueMaterialBuffer", res, 0, 1);
    buffer->Create_attachement(Pixel::SizedFormat::Uint8_NormalizedRGB); //Color
    buffer->Create_attachement(Pixel::SizedFormat::Float16_RGB); //Emissive
    //buffer->Create_attachement(GL_RED, GL_R8);
    return buffer;
}

auto CreateTransparentMaterialBuffer(const std::string& name, glm::ivec2 res)
{
    auto buffer = Component::Create<Framebuffer>(name, res, 0, 1);
    buffer->Create_attachement(Pixel::SizedFormat::Float16_RGBA); // Color;
    buffer->Create_attachement(Pixel::SizedFormat::Uint8_NormalizedR); // Alpha coverage;
    buffer->Create_attachement(Pixel::SizedFormat::Float16_RGB); //Emissive
    buffer->Create_attachement(Pixel::SizedFormat::Uint8_NormalizedRGB); // Distortion direction XY, roughness
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
    glDisable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    Render::DisplayQuad()->Draw();
    opaqueBuffer->bind(false);
    compositing_shader->use(false);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

std::shared_ptr<Framebuffer> OpaquePass(const RenderHistory& lastRender)
{
    glm::ivec2 res = glm::vec2(Window::GetSize()) * Config::Global().Get("InternalQuality", 1.f);
    glm::vec2 geometryRes = glm::vec2(std::min(res.x, res.y));
    static auto geometryBuffer = CreateGeometryBuffer("GeometryBuffer", geometryRes);
    static auto lightingBuffer = CreateLightingBuffer("LightingBuffer", geometryRes);
    static auto opaqueBuffer = CreateOpaqueMaterialBuffer(res);
    static auto transparentBuffer = CreateTransparentMaterialBuffer("TransparentBuffer0", res);
    static auto transparentBuffer1 = CreateTransparentMaterialBuffer("TransparentBuffer1", res / 2);
    auto fastTransparency{ Config::Global().Get("FastTransparency", 1) };
    Render::Private::Instance()._opaqueBuffer = opaqueBuffer;
    Render::Private::Instance()._lightBuffer = lightingBuffer;
    Render::Private::Instance()._geometryBuffer = geometryBuffer;
    geometryBuffer->Resize(geometryRes);
    lightingBuffer->Resize(geometryRes);
    opaqueBuffer->Resize(res);
    transparentBuffer->Resize(res);
    if (fastTransparency)
        transparentBuffer1->Resize(res);
    else
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

    if (Config::Global().Get("SSRQuality", 4) > 0 && lastRender.framebuffer != nullptr)
        SSRPass(geometryBuffer, lastRender, lightingBuffer);
    if (Config::Global().Get("SSAOQuality", 4) > 0)
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

    transparentBuffer1->bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    transparentBuffer1->bind(false);

    if (!fastTransparency) {
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
        transparentBuffer->BlitTo(transparentBuffer1, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
    else
        opaqueBuffer->BlitTo(transparentBuffer1, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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
    glEnable(GL_BLEND);
    if (!fastTransparency) {
        glDepthFunc(GL_GREATER);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2.0, -2.0);
    }
    else {
        glDepthFunc(GL_LESS);
    }
    Scene::Current()->Render(RenderPass::Material, RenderMod::RenderTransparent);
    transparentBuffer1->bind(false);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(2, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(3, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    opaqueBuffer->attachement(0)->GenerateMipmap();
    opaqueBuffer->attachement(1)->GenerateMipmap();
    opaqueBuffer->attachement(0)->SetParameter<Texture::Parameter::BaseLevel>(1);
    opaqueBuffer->attachement(1)->SetParameter<Texture::Parameter::BaseLevel>(1);
    CompositingPass(opaqueBuffer, transparentBuffer1);
    opaqueBuffer->attachement(0)->SetParameter<Texture::Parameter::BaseLevel>(0);
    opaqueBuffer->attachement(1)->SetParameter<Texture::Parameter::BaseLevel>(0);
    if (!fastTransparency) {
        opaqueBuffer->attachement(0)->GenerateMipmap();
        opaqueBuffer->attachement(1)->GenerateMipmap();
        opaqueBuffer->attachement(0)->SetParameter<Texture::Parameter::BaseLevel>(1);
        opaqueBuffer->attachement(1)->SetParameter<Texture::Parameter::BaseLevel>(1);
        CompositingPass(opaqueBuffer, transparentBuffer);
        opaqueBuffer->attachement(0)->SetParameter<Texture::Parameter::BaseLevel>(0);
        opaqueBuffer->attachement(1)->SetParameter<Texture::Parameter::BaseLevel>(0);
    }
    

    return opaqueBuffer;
}

void Render::Private::Scene()
{
    if (Scene::Current() == nullptr || Scene::Current()->CurrentCamera() == nullptr) {
        //present(final_back_buffer);
        return;
    }
    glm::ivec2 res = glm::vec2(Window::GetSize()) * Config::Global().Get("InternalQuality ", 1.f);
    static RenderHistory renderHistory;
    auto renderBuffer(OpaquePass(renderHistory));
    if (renderHistory.framebuffer == nullptr)
        renderHistory.framebuffer = CreateHistoryBuffer(renderBuffer->Size());
    //renderBuffer->attachement(1)->blur(Config::Global().Get("BloomPass", 1), 5);

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
    Render::DisplayQuad()->Draw();
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
    finalRenderBuffer->Resize(Window::GetSize());
    glDisable(GL_BLEND);

    TemporalAccumulationShader->SetTexture("in_renderHistory.color", renderHistory.framebuffer->attachement(0));
    TemporalAccumulationShader->SetUniform("in_renderHistory.viewMatrix", renderHistory.viewMatrix);
    TemporalAccumulationShader->SetUniform("in_renderHistory.projectionMatrix", renderHistory.projectionMatrix);
    TemporalAccumulationShader->SetTexture("in_CurrentColor", renderBuffer->attachement(0));
    TemporalAccumulationShader->SetTexture("in_CurrentVelocity", Render::GeometryBuffer()->attachement(6));
    TemporalAccumulationShader->SetTexture("Texture.Geometry.Normal", Render::GeometryBuffer()->attachement(4));
    TemporalAccumulationShader->SetTexture("Texture.Geometry.Depth", renderBuffer->depth());
    finalRenderBuffer->bind();
    TemporalAccumulationShader->use();
    Render::DisplayQuad()->Draw();
    TemporalAccumulationShader->use(false);
    
    renderHistory.framebuffer->Resize(renderBuffer->Size());
    renderHistory.viewMatrix = Scene::Current()->CurrentCamera()->ViewMatrix();
    renderHistory.projectionMatrix = Scene::Current()->CurrentCamera()->ProjectionMatrix();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_DEPTH_TEST);
    finalRenderBuffer->BlitTo(renderHistory.framebuffer, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glDepthMask(GL_FALSE);

    finalRenderBuffer->BlitTo(nullptr, GL_COLOR_BUFFER_BIT);
    Window::Swap();
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

void Render::Init()
{
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    Private::Instance()._context = SDL_GL_CreateContext((SDL_Window*)Window::GetHandle());
    if (Private::Instance()._context == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
    glewExperimental = GL_TRUE;
    auto error = glewInit();
    if (error != GLEW_OK) {
        throw std::runtime_error(reinterpret_cast<const char*>(glewGetErrorString(error)));
    }
    debugLog(std::string("GL vendor    : ") + reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    debugLog(std::string("GL renderer  : ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    debugLog(std::string("GL version   : ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    debugLog(std::string("GLSL version : ") + reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#ifdef DEBUG_MOD
    PrintExtensions();
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, GL_DONT_CARE, nullptr, GL_FALSE);
    glDebugMessageCallback(MessageCallback, 0);
#endif
}

Render::Context Render::GetContext()
{
    return Private::Instance()._context;
}

void Render::RequestRedraw()
{
    Render::Private::Instance()._needsUpdate = true;;
}

void Render::AddPostTreatment(std::shared_ptr<Shader> shader)
{
    Render::Private::AddPostTreatment(shader);
}

void Render::RemovePostTreatment(std::shared_ptr<Shader> shader)
{
    Render::Private::RemovePostTreatment(shader);
}

std::atomic<bool>& Render::NeedsUpdate()
{
    return Render::Private::Instance()._needsUpdate;
}

std::atomic<bool>& Render::Drawing()
{
    return Render::Private::Instance()._drawing;
}

Signal<float>& Render::OnBeforeRender()
{
    return Render::Private::Instance()._onBeforeRender;
}

Signal<float>& Render::OnAfterRender()
{
    return Render::Private::Instance()._onAfterRender;
}

void Render::Scene()
{
    //static uint32_t frameNbr { 0 };
    double ticks { SDL_GetTicks() / 1000.0 };
    static double lastTicks;
    Render::Private::Instance()._frameNbr++;
    Shader::SetGlobalUniform("FrameNumber", Render::Private::Instance()._frameNbr);
    Shader::SetGlobalUniform("Camera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
    Shader::SetGlobalUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
    Shader::SetGlobalUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
    Shader::SetGlobalUniform("Camera.InvMatrix.View", glm::inverse(Scene::Current()->CurrentCamera()->ViewMatrix()));
    Shader::SetGlobalUniform("Camera.InvMatrix.Projection", glm::inverse(Scene::Current()->CurrentCamera()->ProjectionMatrix()));
    lastTicks = ticks;
    ticks = SDL_GetTicks() / 1000.0;
    Render::OnBeforeRender()(ticks - lastTicks);
    Render::Private::Scene();
    lastTicks = ticks;
    ticks = SDL_GetTicks() / 1000.0;
    Render::OnBeforeRender()(ticks - lastTicks);
    Shader::SetGlobalUniform("PrevCamera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
    Shader::SetGlobalUniform("PrevCamera.Matrix.View", Scene::Current()->CurrentCamera()->ViewMatrix());
    Shader::SetGlobalUniform("PrevCamera.Matrix.Projection", Scene::Current()->CurrentCamera()->ProjectionMatrix());
    Shader::SetGlobalUniform("PrevCamera.InvMatrix.View", glm::inverse(Scene::Current()->CurrentCamera()->ViewMatrix()));
    Shader::SetGlobalUniform("PrevCamera.InvMatrix.Projection", glm::inverse(Scene::Current()->CurrentCamera()->ProjectionMatrix()));
}

const std::shared_ptr<Framebuffer> Render::OpaqueBuffer()
{
    return Render::Private::Instance()._opaqueBuffer;
}

const std::shared_ptr<Framebuffer> Render::LightBuffer()
{
    return Render::Private::Instance()._lightBuffer;
}

const std::shared_ptr<Framebuffer> Render::GeometryBuffer()
{
    return Render::Private::Instance()._geometryBuffer;
}

uint32_t Render::FrameNumber()
{
    return Render::Private::Instance()._frameNbr;
}
