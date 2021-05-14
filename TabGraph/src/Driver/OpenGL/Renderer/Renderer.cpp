/*
* @Author: gpinchon
* @Date:   2021-03-26 14:20:39
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-30 01:59:31
*/

#include "Driver/OpenGL/Renderer/Renderer.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Camera/Camera.hpp"
#include "Config.hpp"
#include "Framebuffer.hpp"
#include "Light/Light.hpp"
#include "Mesh/Geometry.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/SceneRenderer.hpp"
#include "Renderer/SkyboxRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Shader/Global.hpp"
#include "Shader/Program.hpp"
#include "Skybox.hpp"
#include "Texture/PixelUtils.hpp"
#include "Texture/Texture2D.hpp"
#include "Texture/TextureSampler.hpp"
#include "Window.hpp"
#include "brdfLUT.hpp"

#include "Driver/OpenGL/Texture/Texture.hpp"

#include <GL/glew.h> // for GL_TEXTURE0, glDepthFunc, glClear, glDis...
#include <SDL_timer.h> // for SDL_GetTicks
#include <SDL_video.h> // for SDL_GL_MakeCurrent, SDL_GL_SetSwapInterval

#ifdef DEBUG_MOD
void PrintExtensions()
{
    debugLog("GL Extensions :");
    GLint n;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for (auto i = 0; i < n; ++i) {
        debugLog(glGetStringi(GL_EXTENSIONS, i));
    }
}
#endif

static inline auto CreateDeferredRenderBuffer(const std::string& name, const glm::ivec2& size)
{
    auto depthStencilBuffer { Component::Create<Texture2D>(size, Pixel::SizedFormat::Depth24_Stencil8) };
    auto buffer = Component::Create<Framebuffer>(name, size);
    buffer->AddColorBuffer(Pixel::SizedFormat::Uint8_NormalizedRGBA); // BRDF CDiff, Ambient Occlusion
    buffer->AddColorBuffer(Pixel::SizedFormat::Uint8_NormalizedRGBA); // BRDF F0, BRDF Alpha
    buffer->AddColorBuffer(Pixel::SizedFormat::Int16_NormalizedRGB); // Normal
    buffer->AddColorBuffer(Pixel::SizedFormat::Float16_RG); //Velocity
    buffer->AddColorBuffer(Pixel::SizedFormat::Float16_RGB); // Color (Unlit/Emissive/Final Color)
    buffer->SetDepthBuffer(depthStencilBuffer);
    //buffer->SetStencilBuffer(depthStencilBuffer);
    buffer->GetColorBuffer(0)->SetAutoMipMap(false);
    buffer->GetColorBuffer(1)->SetAutoMipMap(false);
    buffer->GetColorBuffer(2)->SetAutoMipMap(false);
    buffer->GetColorBuffer(3)->SetAutoMipMap(false);
    buffer->GetColorBuffer(4)->SetAutoMipMap(false);
    buffer->GetColorBuffer(0)->SetMipMapNbr(1);
    buffer->GetColorBuffer(1)->SetMipMapNbr(1);
    buffer->GetColorBuffer(2)->SetMipMapNbr(1);
    buffer->GetColorBuffer(3)->SetMipMapNbr(1);
    buffer->GetColorBuffer(4)->SetMipMapNbr(1);
    depthStencilBuffer->SetAutoMipMap(true);
    return (buffer);
}

static inline auto CreateLightingBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Component::Create<Framebuffer>(name, size);
    buffer->AddColorBuffer(Pixel::SizedFormat::Float16_RGB); //Diffuse
    buffer->AddColorBuffer(Pixel::SizedFormat::Float16_RGB); //Reflection
    buffer->GetColorBuffer(0)->SetAutoMipMap(false);
    buffer->GetColorBuffer(1)->SetAutoMipMap(false);
    buffer->GetColorBuffer(0)->SetMipMapNbr(1);
    buffer->GetColorBuffer(1)->SetMipMapNbr(1);
    return buffer;
}

static inline auto CreateTransparentForwardRenderBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Component::Create<Framebuffer>(name, size);
    buffer->AddColorBuffer(Pixel::SizedFormat::Float16_RGBA); //Color
    buffer->AddColorBuffer(Pixel::SizedFormat::Uint8_NormalizedR); //Alpha Coverage
    buffer->AddColorBuffer(Pixel::SizedFormat::Uint8_NormalizedRGB); //Distortion
    buffer->AddColorBuffer(Pixel::SizedFormat::Uint8_NormalizedRGBA); //Transmission Color (RGB), BRDF Alpha(A)
    buffer->GetColorBuffer(0)->SetAutoMipMap(false);
    buffer->GetColorBuffer(1)->SetAutoMipMap(false);
    buffer->GetColorBuffer(2)->SetAutoMipMap(false);
    buffer->GetColorBuffer(3)->SetAutoMipMap(false);
    buffer->GetColorBuffer(0)->SetMipMapNbr(1);
    buffer->GetColorBuffer(1)->SetMipMapNbr(1);
    buffer->GetColorBuffer(2)->SetMipMapNbr(1);
    buffer->GetColorBuffer(3)->SetMipMapNbr(1);
    return buffer;
}

static inline auto CreateOpaqueRenderBuffer(const std::string& name, const glm::ivec2& size)
{
    auto buffer = Component::Create<Framebuffer>(name, size);
    buffer->AddColorBuffer(Pixel::SizedFormat::Float16_RGB); //Color
    buffer->GetColorBuffer(0)->SetAutoMipMap(true);
    return buffer;
}

static inline auto CreateFinalRenderBuffer(const std::string& name, glm::ivec2 res)
{
    auto buffer = Component::Create<Framebuffer>(name, res);
    buffer->AddColorBuffer(Pixel::SizedFormat::Uint8_NormalizedRGB); //Color
    buffer->GetColorBuffer(0)->SetAutoMipMap(false);
    buffer->GetColorBuffer(0)->SetMipMapNbr(1);
    return buffer;
}

Renderer::Impl::Impl()
{
}

void Renderer::Impl::Init()
{
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    _context = SDL_GL_CreateContext((SDL_Window*)Window::GetHandle());
    if (_context == nullptr) {
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

    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#ifdef DEBUG_MOD
    PrintExtensions();
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, GL_DONT_CARE, nullptr, GL_FALSE);
    glDebugMessageCallback(MessageCallback, 0);
#endif
    _deferredLightingBuffer = CreateLightingBuffer("DeferredLightingBuffer", glm::ivec2(256));
    _deferredRenderBuffer = CreateDeferredRenderBuffer("DeferredRenderBuffer", glm::ivec2(256));
    _forwardTransparentRenderBuffer = CreateTransparentForwardRenderBuffer("TransparentForwardRenderBuffer", glm::ivec2(256));
    _opaqueRenderBuffer = CreateOpaqueRenderBuffer("OpaqueForwardRenderBuffer", glm::ivec2(256));
    _finalRenderBuffer = CreateFinalRenderBuffer("FinalRenderBuffer", glm::ivec2(256));
    _previousRenderBuffer = CreateFinalRenderBuffer("FinalRenderBuffer", glm::ivec2(256));
}

Renderer::Context& Renderer::Impl::GetContext()
{
    return GetImpl()->GetContext();
}

void Renderer::Impl::_HZBPass()
{
    auto depthTexture { std::static_pointer_cast<Texture2D>(DeferredGeometryBuffer()->GetDepthBuffer()) };
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_DEPTH_TEST);
    //static std::shared_ptr<Shader::Program> HZBShader;
    if (_hzbShader == nullptr) {
        auto PassThroughVertexCode =
#include "passthrough.vert"
            ;
        auto HZBFragmentCode =
#include "hzb.frag"
            ;
        auto ShaderVertexCode = Shader::Stage::Code { PassThroughVertexCode, "PassThrough();" };
        auto ShaderFragmentCode = Shader::Stage::Code { HZBFragmentCode, "HZB();" };
        _hzbShader = Component::Create<Shader::Program>("HZB");
        _hzbShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, ShaderVertexCode));
        _hzbShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, ShaderFragmentCode));
    }
    if (_hzbBuffer == nullptr) {
        _hzbBuffer = Component::Create<Framebuffer>("HZB", depthTexture->GetSize(), 0, 0);
    }
    //static auto framebuffer = Component::Create<Framebuffer>("HZB", depthTexture->GetSize(), 0, 0);
    depthTexture->GenerateMipmap();
    depthTexture->GetTextureSampler()->SetMinFilter(TextureSampler::Filter::LinearMipmapLinear);
    auto numLevels = depthTexture->GetMipMapNbr(); //1 + unsigned(floorf(log2f(fmaxf(depthTexture->GetSize().x, depthTexture->GetSize().y))));
    auto currentSize = depthTexture->GetSize();
    for (auto i = 1; i < numLevels; i++) {
        depthTexture->GetTextureSampler()->SetMinLOD(i - 1);
        depthTexture->GetTextureSampler()->SetMaxLOD(i - 1);
        currentSize /= 2;
        currentSize = glm::max(currentSize, glm::ivec2(1));
        _hzbBuffer->Resize(currentSize);
        //framebuffer->set_attachement(0, depthTexture, i);
        _hzbBuffer->SetDepthBuffer(depthTexture, i);
        _hzbBuffer->bind();
        _hzbShader->Use()
            .SetTexture("in_Texture_Color", depthTexture);
        Renderer::Render(Renderer::GetImpl()->DisplayQuad(), true);
        _hzbShader->Done();
        _hzbBuffer->SetDepthBuffer(nullptr);
        //framebuffer->set_attachement(0, nullptr);
    }
    depthTexture->GetTextureSampler()->SetMinLOD(0);
    depthTexture->GetTextureSampler()->SetMaxLOD(numLevels - 1);
    depthTexture->GetTextureSampler()->SetMinFilter(TextureSampler::Filter::LinearMipmapLinear);
    _hzbBuffer->bind(false);
}

static inline auto DeferredVertexCode()
{
    auto deferred_vert_code =
#include "deferred.vert"
        ;
    return Shader::Stage::Code { deferred_vert_code, "FillVertexData();" };
}

static inline auto DeferredFragmentCode()
{
    auto deferred_frag_code =
#include "deferred.frag"
        ;
    return Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" };
}

auto PassThroughShader()
{
    static std::shared_ptr<Shader::Program> passthroughShader;
    if (passthroughShader == nullptr) {
        auto passthroughVertexCode =
#include "passthrough.vert"
            ;
        auto passthroughFragmentCode =
#include "passthrough.frag"
            ;
        passthroughShader = Component::Create<Shader::Program>("compositing");
        passthroughShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { passthroughVertexCode, "PassThrough();" }));
        passthroughShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { passthroughFragmentCode, "PassThrough();" }));
        Renderer::GetImpl()->AddComponent(passthroughShader);
    }
    return passthroughShader;
}

void Renderer::Impl::_SSAOPass()
{
    const auto gBuffer { Renderer::DeferredGeometryBuffer() };
    glm::ivec2 res = glm::ivec2(std::min(gBuffer->Size().x, gBuffer->Size().y)); // glm::vec2(1024) * Renderer::Private::InternalQuality();// *Config::Global().Get("SSAOResolutionFactor", 0.5f);
    glDepthMask(false);
    if (_ssaoBuffer == nullptr) {
        _ssaoBuffer = Component::Create<Framebuffer>("SSAOBuffer", res);
        _ssaoBuffer->AddColorBuffer(Pixel::SizedFormat::Uint8_NormalizedRGBA);
    }
    if (_ssaoShader == nullptr) {
        auto SSAOFragmentCode =
#include "ssao.frag"
            ;
        _ssaoShader = Component::Create<Shader::Program>("SSAO");
        _ssaoShader->Attach({ Shader::Stage::Type::Vertex, DeferredVertexCode() });
        _ssaoShader->Attach({ Shader::Stage::Type::Fragment, DeferredFragmentCode() + Shader::Stage::Code { SSAOFragmentCode, "SSAO();" } });
        _ssaoShader->SetDefine("Pass", "GeometryPostTreatment");
    }
    _ssaoShader->SetDefine("SSAO_QUALITY", std::to_string(Config::Global().Get("SSAOQuality", 4)));

    _ssaoBuffer->Resize(res);
    _ssaoBuffer->bind();
    _ssaoShader->Use()
        .SetTexture("Texture.Geometry.Normal", gBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", gBuffer->GetDepthBuffer());
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    Renderer::Render(Renderer::DisplayQuad(), true);

    PassThroughShader()->Use().SetTexture("in_ColorBuffer0", _ssaoBuffer->GetColorBuffer(0)).SetUniform("in_UseColorBuffer0", true);
    gBuffer->bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    Renderer::Render(Renderer::DisplayQuad(), true);
    gBuffer->bind(false);
    //reset global shader uniform
    PassThroughShader()->SetTexture("in_ColorBuffer0", nullptr).SetUniform("in_UseColorBuffer0", false).Done();
}

static inline float ComputeRoughnessMaskScale()
{
    float MaxRoughness = std::clamp(Config::Global().Get("SSRMaxRoughness", 0.8f), 0.01f, 1.0f);

    float RoughnessMaskScale = -2.0f / MaxRoughness;
    return RoughnessMaskScale * (Config::Global().Get("SSRQuality", 4) < 3 ? 2.0f : 1.0f);
}

void Renderer::Impl::_SSRPass()
{
    const auto gBuffer { DeferredGeometryBuffer() };
    const auto lBuffer { DeferredLightingBuffer() };
    const auto res { glm::ivec2(std::min(gBuffer->Size().x, gBuffer->Size().y)) };
    if (_ssrBuffer == nullptr) {
        _ssrBuffer = Component::Create<Framebuffer>("SSRBuffer", res);
        _ssrBuffer->AddColorBuffer(nullptr)
            .AddColorBuffer(Pixel::SizedFormat::Float16_RGBA)
            .SetStencilBuffer(Component::Create<Texture2D>(res, Pixel::SizedFormat::Stencil8));
    }
    _ssrBuffer->Resize(res);
    if (_ssrShader == nullptr) {
        auto deferred_vert_code =
#include "deferred.vert"
            ;
        auto RandomCode =
#include "Random.glsl"
            ;
        auto SSRFragmentCode =
#include "SSR.frag"
            ;
        auto deferred_frag_code =
#include "deferred.frag"
            ;
        _ssrShader = Component::Create<Shader::Program>("SSR0");
        _ssrShader->SetDefine("Pass", "DeferredLighting");
        _ssrShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment,
            Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { RandomCode } + Shader::Stage::Code { SSRFragmentCode, "SSR();" }));
        _ssrShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex,
            Shader::Stage::Code { deferred_vert_code, "FillVertexData();" }));
    }
    if (_ssrApplyShader == nullptr) {
        auto SSRApplyFragmentCode =
#include "SSRApply.frag"
            ;
        auto deferred_vert_code =
#include "deferred.vert"
            ;
        auto deferred_frag_code =
#include "deferred.frag"
            ;
        _ssrApplyShader = Component::Create<Shader::Program>("SSR0");
        _ssrApplyShader->SetDefine("Pass", "DeferredLighting");
        _ssrApplyShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment,
            Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { SSRApplyFragmentCode, "SSRApply();" }));
        _ssrApplyShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex,
            Shader::Stage::Code { deferred_vert_code, "FillVertexData();" }));
    }
    if (_ssrStencilShader == nullptr) {
        auto SSRStencilFragmentCode =
#include "SSRStencil.frag"
            ;
        auto deferred_vert_code =
#include "deferred.vert"
            ;
        auto deferred_frag_code =
#include "deferred.frag"
            ;
        _ssrStencilShader = Component::Create<Shader::Program>("SSR0");
        _ssrStencilShader->SetDefine("Pass", "DeferredLighting");
        _ssrStencilShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment,
            Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { SSRStencilFragmentCode, "SSRStencil();" }));
        _ssrStencilShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex,
            Shader::Stage::Code { deferred_vert_code, "FillVertexData();" }));
    }
    _ssrShader->GetStage(Shader::Stage::Type::Fragment).SetDefine("SSR_QUALITY", std::to_string(Config::Global().Get("SSRQuality", 4)));
    _ssrShader->GetStage(Shader::Stage::Type::Fragment).SetDefine("SCREEN_BORDER_FACTOR", std::to_string(Config::Global().Get("SSRBorderFactor", 10)));
    _ssrShader->GetStage(Shader::Stage::Type::Fragment).SetDefine("ROUGHNESSMASKSCALE", std::to_string(ComputeRoughnessMaskScale()));
    _ssrStencilShader->GetStage(Shader::Stage::Type::Fragment).SetDefine("ROUGHNESSMASKSCALE", std::to_string(ComputeRoughnessMaskScale()));

    _ssrBuffer->bind();
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glClearStencil(0x00);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilOp(
        GL_KEEP,
        GL_KEEP,
        GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0x1, 0xff);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    _ssrStencilShader->Use()
        .SetTexture("Texture.Geometry.F0", gBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Depth", gBuffer->GetDepthBuffer());
    Renderer::Render(Renderer::DisplayQuad(), true);

    glStencilMask(0x00);
    glStencilOp(
        GL_KEEP,
        GL_KEEP,
        GL_KEEP);
    glStencilFunc(GL_EQUAL, 0x1, 0xff);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    _ssrShader->Use()
        .SetTexture("Texture.Geometry.F0", gBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", gBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", gBuffer->GetDepthBuffer())
        .SetTexture("LastColor", Renderer::PreviousRenderBuffer()->GetColorBuffer(0));
    Renderer::Render(Renderer::DisplayQuad(), true);
    glDisable(GL_STENCIL_TEST);

    lBuffer->bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    _ssrApplyShader->Use()
        .SetTexture("Texture.Geometry.F0", gBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", gBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Depth", gBuffer->GetDepthBuffer())
        .SetTexture("SSRTexture", _ssrBuffer->GetColorBuffer(1));
    Renderer::Render(Renderer::DisplayQuad(), true);
    _ssrApplyShader->Done();
    lBuffer->bind(false);

    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
}

void Renderer::Impl::_DeferredMaterialPass()
{
    if (_deferredMaterialShader == nullptr) {
        auto deferredMaterialCode =
#include "deferredMaterial.frag"
            ;
        auto deferred_vert_code =
#include "deferred.vert"
            ;
        auto deferred_frag_code =
#include "deferred.frag"
            ;
        _deferredMaterialShader = Component::Create<Shader::Program>("SSR0");
        _deferredMaterialShader->SetDefine("Pass", "DeferredMaterial");
        _deferredMaterialShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment,
            Shader::Stage::Code { deferred_frag_code, "FillFragmentData();" } + Shader::Stage::Code { deferredMaterialCode, "CalculateLighting();" }));
        _deferredMaterialShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex,
            Shader::Stage::Code { deferred_vert_code, "FillVertexData();" }));
    }
    auto gBuffer { DeferredGeometryBuffer() };
    auto lBuffer { DeferredLightingBuffer() };
    auto oBuffer { OpaqueRenderBuffer() };
    oBuffer->bind();
    glDisable(GL_BLEND);
    PassThroughShader()->Use().SetTexture("in_ColorBuffer0", gBuffer->GetColorBuffer(4)).SetUniform("in_UseColorBuffer0", true);
    Renderer::Render(Renderer::DisplayQuad(), true);
    PassThroughShader()->SetTexture("in_ColorBuffer0", nullptr).SetUniform("in_UseColorBuffer0", false);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    _deferredMaterialShader->Use()
        .SetTexture("Texture.Geometry.CDiff", gBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Geometry.F0", gBuffer->GetColorBuffer(1))
        .SetTexture("Texture.Geometry.Normal", gBuffer->GetColorBuffer(2))
        .SetTexture("Texture.Geometry.Velocity", gBuffer->GetColorBuffer(3))
        .SetTexture("Texture.Geometry.Color", gBuffer->GetColorBuffer(4))
        .SetTexture("Texture.Geometry.Depth", gBuffer->GetDepthBuffer())
        .SetTexture("Texture.Lighting.Diffuse", lBuffer->GetColorBuffer(0))
        .SetTexture("Texture.Lighting.Reflection", lBuffer->GetColorBuffer(1))
        //.SetTexture("Texture.Geometry.Diffuse", renderBuffer->attachement(6))
        //.SetTexture("Texture.Geometry.Reflection", renderBuffer->attachement(7))
        .SetTexture("BRDFLUT", Renderer::DefaultBRDFLUT());
    Renderer::Render(Renderer::DisplayQuad(), true);
    oBuffer->bind(false);
    _deferredMaterialShader->Done();
}

#include "Renderer/Light/LightRenderer.hpp"

void light_pass(std::shared_ptr<Framebuffer>& lightingBuffer)
{
    auto& lights { Scene::Current()->GetComponents<Light>() };
    for (auto& light : lights) {
        Renderer::Render(light, { Renderer::Options::Pass::ShadowDepth, Renderer::Options::Mode::All, Scene::Current()->CurrentCamera(), Scene::Current(), Renderer::FrameNumber() });
        //light->render_shadow();
    }
    lightingBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    glCullFace(GL_FRONT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    for (auto& light : lights) {
        Renderer::Render(light, { Renderer::Options::Pass::DeferredLighting, Renderer::Options::Mode::All, Scene::Current()->CurrentCamera(), Scene::Current(), Renderer::FrameNumber() });
        //light->Draw();
    }
    lightingBuffer->bind(false);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
}

void Renderer::Impl::_OpaquePass()
{
    auto gBuffer { DeferredGeometryBuffer() };
    auto lBuffer { DeferredLightingBuffer() };
    auto oBuffer { OpaqueRenderBuffer() };
    //Geometry pass
    {
        gBuffer->bind();
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
        Renderer::Render(Scene::Current(), { Renderer::Options::Pass::DeferredGeometry, Renderer::Options::Mode::All, Scene::Current()->CurrentCamera(), Scene::Current(), Renderer::FrameNumber() });
        gBuffer->bind(false);
    }
    //SSAO & SSR pass
    {
        if (Config::Global().Get("SSAOQuality", 4) > 0)
            _SSAOPass();
        light_pass(lBuffer);
        _HZBPass();
        if (Config::Global().Get("SSRQuality", 4) > 0)
            _SSRPass();
    }
    _DeferredMaterialPass();
}

void Renderer::Impl::_CompositingPass()
{
    auto oBuffer { OpaqueRenderBuffer() };
    auto tBuffer { ForwardTransparentRenderBuffer() };
    if (_compositingShader == nullptr) {
        auto passthrough_vertex_code =
#include "passthrough.vert"
            ;
        auto compositingShaderCode =
#include "compositing.frag"
            ;
        _compositingShader = Component::Create<Shader::Program>("compositing");
        _compositingShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, { passthrough_vertex_code, "PassThrough();" }));
        _compositingShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment, { compositingShaderCode, "Composite();" }));
    }
    oBuffer->GetColorBuffer(0)->GenerateMipmap();
    oBuffer->GetColorBuffer(0)->GetTextureSampler()->SetMinLOD(1);
    oBuffer->GetColorBuffer(0)->GetTextureSampler()->SetMinFilter(TextureSampler::Filter::LinearMipmapLinear);
    oBuffer->SetStencilBuffer(tBuffer->GetStencilBuffer());

    oBuffer->bind();
    _compositingShader->Use()
        .SetTexture("in_TransparentColor", tBuffer->GetColorBuffer(0))
        .SetTexture("in_TransparentAlphaCoverage", tBuffer->GetColorBuffer(1))
        .SetTexture("in_TransparentDistortion", tBuffer->GetColorBuffer(2))
        .SetTexture("in_TransparentTransmission", tBuffer->GetColorBuffer(3))
        .SetTexture("in_OpaqueColor", oBuffer->GetColorBuffer(0));
    glStencilOp(
        GL_KEEP,
        GL_KEEP,
        GL_KEEP);
    glStencilFunc(GL_EQUAL, 0x1, 0xff);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    Renderer::Render(Renderer::DisplayQuad(), true);
    _compositingShader->Done();
    oBuffer->bind(false);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    oBuffer->GetColorBuffer(0)->GetTextureSampler()->SetMinLOD(0);
    oBuffer->GetColorBuffer(0)->GetTextureSampler()->SetMinFilter(TextureSampler::Filter::LinearMipmapNearest);
    oBuffer->SetStencilBuffer(nullptr);
}

void Renderer::Impl::_TransparentPass()
{
    auto zero = glm::vec4(0);
    auto one = glm::vec4(1);
    auto fastTransparency { Config::Global().Get("FastTransparency", 1) };
    auto gBuffer { DeferredGeometryBuffer() };
    auto lBuffer { DeferredLightingBuffer() };
    auto oBuffer { OpaqueRenderBuffer() };
    auto tBuffer { ForwardTransparentRenderBuffer() };

    tBuffer->SetStencilBuffer(gBuffer->GetDepthBuffer());
    tBuffer->SetDepthBuffer(gBuffer->GetDepthBuffer());
    tBuffer->bind();
    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(2, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColorMaski(3, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearBufferfv(GL_COLOR, 0, &zero[0]);
    glClearBufferfv(GL_COLOR, 1, &one[0]);
    glClearBufferfv(GL_COLOR, 2, &zero[0]);
    glClearBufferfv(GL_COLOR, 3, &zero[0]);
    //Setup blending
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glBlendFunci(2, GL_ONE, GL_ONE);
    glBlendFuncSeparatei(3,
        GL_ONE, GL_ONE_MINUS_SRC_COLOR,
        GL_ONE, GL_ONE);
    //glBlendFunci(3, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    //Setup depth test
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LESS);
    //Setup stencil test
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x1);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilOp(
        GL_KEEP,
        GL_KEEP,
        GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0x1, 0xff);

    Renderer::Render(Scene::Current(), { Renderer::Options::Pass::ForwardTransparent, Renderer::Options::Mode::Transparent, Scene::Current()->CurrentCamera(), Scene::Current(), Renderer::FrameNumber() });
    _CompositingPass();
}

void Renderer::Impl::_RenderFrame()
{
    if (Scene::Current() == nullptr || Scene::Current()->CurrentCamera() == nullptr) {
        //present(final_back_buffer);
        return;
    }
    auto fBuffer { Renderer::FinalRenderBuffer() };
    auto gBuffer { Renderer::DeferredGeometryBuffer() };
    auto lBuffer { Renderer::DeferredLightingBuffer() };
    auto oBuffer { Renderer::OpaqueRenderBuffer() };
    auto tBuffer { Renderer::ForwardTransparentRenderBuffer() };
    glm::ivec2 res = glm::vec2(Window::GetSize()) * Config::Global().Get("InternalQuality", 1.f);
    fBuffer->Resize(res);
    gBuffer->Resize(res);
    lBuffer->Resize(res);
    oBuffer->Resize(res);
    tBuffer->Resize(res);
    gBuffer->bind();
    glDepthMask(GL_TRUE);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    if (Scene::Current()->GetSkybox() != nullptr)
        Renderer::Render(Scene::Current()->GetSkybox(), { Renderer::Options::Pass::ForwardOpaque, Renderer::Options::Mode::All, Scene::Current()->CurrentCamera(), Scene::Current(), Renderer::FrameNumber() });
    gBuffer->bind(false);
    _OpaquePass();
    _TransparentPass();
    static std::shared_ptr<Shader::Program> TemporalAccumulationShader;
    if (TemporalAccumulationShader == nullptr) {
        auto TemporalAccumulationShaderCode =
#include "TemporalAccumulation.frag"
            ;
        TemporalAccumulationShader = Component::Create<Shader::Program>("TemporalAccumulation");
        TemporalAccumulationShader->SetDefine("MATERIAL");
        TemporalAccumulationShader->Attach(Shader::Stage(Shader::Stage::Type::Vertex, DeferredVertexCode()));
        TemporalAccumulationShader->Attach(Shader::Stage(Shader::Stage::Type::Fragment,
            Shader::Stage::Code { TemporalAccumulationShaderCode, "TemporalAccumulation();" }));
        Renderer::GetImpl()->AddComponent(TemporalAccumulationShader);
    }
    fBuffer->bind();
    glDisable(GL_BLEND);
    TemporalAccumulationShader->Use()
        .SetTexture("in_PreviousColor", Renderer::PreviousRenderBuffer()->GetColorBuffer(0))
        .SetTexture("in_CurrentVelocity", gBuffer->GetColorBuffer(3))
        .SetTexture("in_CurrentColor", oBuffer->GetColorBuffer(0));
    Renderer::Render(Renderer::DisplayQuad(), true);
    TemporalAccumulationShader->Done();
    fBuffer->BlitTo(nullptr, GL_COLOR_BUFFER_BIT);
    Window::Swap();
}

std::shared_ptr<Framebuffer> Renderer::Impl::DeferredGeometryBuffer()
{
    return GetImpl()->_deferredRenderBuffer;
}

std::shared_ptr<Framebuffer> Renderer::Impl::DeferredLightingBuffer()
{
    return GetImpl()->_deferredLightingBuffer;
}

std::shared_ptr<Framebuffer> Renderer::Impl::ForwardTransparentRenderBuffer()
{
    return GetImpl()->_forwardTransparentRenderBuffer;
}

std::shared_ptr<Framebuffer> Renderer::Impl::OpaqueRenderBuffer()
{
    return GetImpl()->_opaqueRenderBuffer;
}

std::shared_ptr<Framebuffer> Renderer::Impl::FinalRenderBuffer()
{
    return GetImpl()->_finalRenderBuffer;
}

std::shared_ptr<Framebuffer> Renderer::Impl::PreviousRenderBuffer()
{
    return GetImpl()->_previousRenderBuffer;
}

std::shared_ptr<Texture2D> Renderer::Impl::DefaultBRDFLUT()
{
    return GetImpl()->_DefaultBRDFLUT();
}

const std::shared_ptr<Geometry> Renderer::Impl::DisplayQuad()
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

const uint32_t Renderer::Impl::FrameNumber()
{
    return GetImpl()->_frameNbr;
}

std::shared_ptr<Texture2D> Renderer::Impl::_DefaultBRDFLUT()
{
    if (_defaultBRDF == nullptr) {
        auto brdfImageAsset(Component::Create<Asset>());
        _defaultBRDF = Component::Create<Texture2D>(brdfImageAsset);
        auto brdfImage { Component::Create<Image>(glm::vec2(256, 256), Pixel::SizedFormat::Uint8_NormalizedRG, brdfLUT) };
        brdfImageAsset->SetComponent(brdfImage);
        brdfImageAsset->SetLoaded(true);
        _defaultBRDF->SetName("BrdfLUT");
        _defaultBRDF->GetTextureSampler()->SetWrapS(TextureSampler::Wrap::ClampToEdge);
        _defaultBRDF->GetTextureSampler()->SetWrapT(TextureSampler::Wrap::ClampToEdge);
    }
    return _defaultBRDF;
}

void Renderer::Impl::RenderFrame()
{
    //static uint32_t frameNbr { 0 };
    double ticks { SDL_GetTicks() / 1000.0 };
    static double lastTicks;
    Renderer::GetImpl()->_frameNbr++;
    auto temp { Renderer::GetImpl()->_finalRenderBuffer };
    Renderer::GetImpl()->_finalRenderBuffer = Renderer::GetImpl()->_previousRenderBuffer;
    Renderer::GetImpl()->_previousRenderBuffer = temp;
    //Shader::Global::SetUniform("Resolution", glm::vec3(Window::GetSize(), Window::GetSize().x / float(Window::GetSize().y)));
    Shader::Global::SetUniform("FrameNumber", Renderer::GetImpl()->_frameNbr);
    Shader::Global::SetUniform("Camera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
    Shader::Global::SetUniform("Camera.Matrix.View", Scene::Current()->CurrentCamera()->GetViewMatrix());
    Shader::Global::SetUniform("Camera.Matrix.Projection", Scene::Current()->CurrentCamera()->GetProjectionMatrix());
    Shader::Global::SetUniform("Camera.InvMatrix.View", glm::inverse(Scene::Current()->CurrentCamera()->GetViewMatrix()));
    Shader::Global::SetUniform("Camera.InvMatrix.Projection", glm::inverse(Scene::Current()->CurrentCamera()->GetProjectionMatrix()));
    lastTicks = ticks;
    ticks = SDL_GetTicks() / 1000.0;
    //Renderer::OnFrameBegin()(Renderer::GetImpl()->_frameNbr, ticks - lastTicks);
    Renderer::OnFrameBegin(Scene::Current(), Renderer::GetImpl()->_frameNbr, ticks - lastTicks);
    Renderer::GetImpl()->_RenderFrame();
    lastTicks = ticks;
    ticks = SDL_GetTicks() / 1000.0;
    //Renderer::OnFrameEnd()(Renderer::GetImpl()->_frameNbr, ticks - lastTicks);
    Renderer::OnFrameEnd(Scene::Current(), Renderer::GetImpl()->_frameNbr, ticks - lastTicks);
    Shader::Global::SetUniform("PrevCamera.Position", Scene::Current()->CurrentCamera()->WorldPosition());
    Shader::Global::SetUniform("PrevCamera.Matrix.View", Scene::Current()->CurrentCamera()->GetViewMatrix());
    Shader::Global::SetUniform("PrevCamera.Matrix.Projection", Scene::Current()->CurrentCamera()->GetProjectionMatrix());
    Shader::Global::SetUniform("PrevCamera.InvMatrix.View", glm::inverse(Scene::Current()->CurrentCamera()->GetViewMatrix()));
    Shader::Global::SetUniform("PrevCamera.InvMatrix.Projection", glm::inverse(Scene::Current()->CurrentCamera()->GetProjectionMatrix()));
}

std::shared_ptr<Component> Renderer::Impl::_Clone()
{
    return Component::Create<Renderer::Impl>(*this);
}

std::shared_ptr<Renderer::Impl> Renderer::GetImpl()
{
    static auto impl { Component::Create<Renderer::Impl>() };
    return impl;
}