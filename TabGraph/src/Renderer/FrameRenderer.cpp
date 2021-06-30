/*
* @Author: gpinchon
* @Date:   2021-06-19 00:47:23
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 00:56:45
*/
#include <Renderer/FrameRenderer.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/FrameRenderer.hpp>
#endif

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

void FrameRenderer::SetViewPort(const glm::ivec2& size)
{
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
