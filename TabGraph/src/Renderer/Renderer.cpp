/*
* @Author: gpinchon
* @Date:   2021-03-22 18:30:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 13:31:16
*/

#include <Camera/Camera.hpp> // for Camera
#include <Config.hpp> // for Config
#include <Debug.hpp>
#include <Engine.hpp> // for UpdateMutex, SwapInterval
#include <Light/Light.hpp> // for Light, Directionnal, Point
#include <Renderer/Renderer.hpp>
#include <Renderer/SceneRenderer.hpp>
#include <Scene/Scene.hpp>
#include <Shader/Global.hpp> // for Shader::Global
#include <Shader/Program.hpp> // for Shader::Program
#include <Surface/Geometry.hpp> // for Geometry
#include <Surface/Skybox.hpp> // for Skybox
#include <Texture/Framebuffer.hpp> // for Framebuffer
#include <Texture/Texture2D.hpp> // for Texture2D
#include <Texture/TextureCubemap.hpp>
#include <Window.hpp> // for Window

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Renderer.hpp>
#endif

#include <algorithm> // for max, remove_if
#include <atomic> // for atomic
#include <glm/glm.hpp> // for glm::inverse, vec2_scale, vec3_scale
#include <iostream> // for char_traits, endl, cout, operator<<, ost...
#include <mutex> // for mutex
#include <stdint.h> // for uint64_t, uint16_t
#include <string> // for operator+, to_string, string
#include <thread> // for thread
#include <vector> // for vector<>::iterator, vector

namespace Renderer {

std::shared_ptr<FrameRenderer> FrameRenderer::Create(std::weak_ptr<Window> window)
{
    std::shared_ptr<FrameRenderer> renderer(new FrameRenderer(window));
    return renderer;
}

void FrameRenderer::SetViewPort(const glm::ivec2& min, const glm::ivec2& max)
{
    _impl->SetViewPort(min, max);
}

void FrameRenderer::SetViewPort(const glm::ivec2& size) {
    return SetViewPort(glm::ivec2(0), size);
}

FrameRenderer::FrameRenderer(std::weak_ptr<Window> window)
    : _impl(new FrameRenderer::Impl(window, *this))
{
}

uint32_t FrameRenderer::GetFrameNumber() const
{
    return _impl->GetFrameNumber();
}

const std::shared_ptr<Window> FrameRenderer::GetWindow() const
{
    return _impl->GetWindow();
}

const std::shared_ptr<Geometry> FrameRenderer::GetDisplayQuad() const
{
    return _impl->GetDisplayQuad();
}

const std::shared_ptr<Texture2D> FrameRenderer::GetDefaultBRDFLUT() const
{
    return _impl->GetDefaultBRDFLUT();
}

std::shared_ptr<Framebuffer> FrameRenderer::DeferredGeometryBuffer()
{
    return _impl->DeferredGeometryBuffer();
}

std::shared_ptr<Framebuffer> FrameRenderer::DeferredLightingBuffer()
{
    return _impl->DeferredLightingBuffer();
}

std::shared_ptr<Framebuffer> FrameRenderer::ForwardTransparentRenderBuffer()
{
    return _impl->ForwardTransparentRenderBuffer();
}

std::shared_ptr<Framebuffer> FrameRenderer::OpaqueRenderBuffer()
{
    return _impl->OpaqueRenderBuffer();
}

std::shared_ptr<Framebuffer> FrameRenderer::FinalRenderBuffer()
{
    return _impl->FinalRenderBuffer();
}

std::shared_ptr<Framebuffer> FrameRenderer::PreviousRenderBuffer()
{
    return _impl->PreviousRenderBuffer();
}

void Renderer::FrameRenderer::RenderFrame(std::shared_ptr<Scene> scene)
{
    _impl->RenderFrame(scene);
}

void FrameRenderer::SetSwapInterval(SwapInterval swapInterval)
{
    return _impl->SetSwapInterval(swapInterval);
}

SwapInterval FrameRenderer::GetSwapInterval() const
{
    return _impl->GetSwapInterval();
}
};
