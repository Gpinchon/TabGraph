/*
* @Author: gpinchon
* @Date:   2021-03-26 14:20:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-02 19:11:43
*/

#pragma once
#include <Renderer/Renderer.hpp>
#include <Component.hpp>
#include <Event/Signal.hpp>

#include <memory>
#include <glm/fwd.hpp>

class Texture2D;
class Framebuffer;
class Geometry;
namespace Shader {
class Program;
};

namespace Renderer {
struct FrameRenderer::Impl : public Component {
    class Context {
    public:
        Context(void* context = nullptr)
            : _v(context)
        {
        }
        operator void* () const
        {
            return _v;
        }

    private:
        void* _v{ nullptr };
    };
    Impl(std::weak_ptr<Window> window, FrameRenderer& renderer);
    const Context& GetContext() const;
    const uint32_t GetFrameNumber() const;
    const std::shared_ptr<Geometry> GetDisplayQuad() const;
    const std::shared_ptr<Texture2D> GetDefaultBRDFLUT() const;
    const std::shared_ptr<Window> GetWindow() const;
    void RenderFrame(std::shared_ptr<Scene> scene);
    
    void SetViewPort(const glm::ivec2& min, const glm::ivec2& max);
    void SetViewPort(const glm::ivec2& size);
    void SetSwapInterval(SwapInterval swapInterval);
    SwapInterval GetSwapInterval() const;

    std::shared_ptr<Framebuffer> DeferredGeometryBuffer();
    std::shared_ptr<Framebuffer> DeferredLightingBuffer();
    std::shared_ptr<Framebuffer> ForwardTransparentRenderBuffer();
    std::shared_ptr<Framebuffer> OpaqueRenderBuffer();
    std::shared_ptr<Framebuffer> FinalRenderBuffer();
    std::shared_ptr<Framebuffer> PreviousRenderBuffer();

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return Component::Create<FrameRenderer::Impl>(*this);
    }
    std::shared_ptr<Framebuffer> _deferredLightingBuffer;
    std::shared_ptr<Framebuffer> _deferredRenderBuffer;
    std::shared_ptr<Framebuffer> _forwardTransparentRenderBuffer;
    std::shared_ptr<Framebuffer> _opaqueRenderBuffer;
    std::shared_ptr<Framebuffer> _finalRenderBuffer;
    std::shared_ptr<Framebuffer> _previousRenderBuffer;

    void _RenderFrame(std::shared_ptr<Scene> scene);
    void _OpaquePass(std::shared_ptr<Scene> scene);
    void _LightPass(std::shared_ptr<Scene> scene);
    void _TransparentPass(std::shared_ptr<Scene> scene);
    void _HZBPass();
    void _SSRPass();
    void _SSAOPass();
    void _DeferredMaterialPass();
    void _CompositingPass();

    std::shared_ptr<Framebuffer> _ssrBuffer;
    std::shared_ptr<Shader::Program> _ssrShader;
    std::shared_ptr<Shader::Program> _ssrStencilShader;
    std::shared_ptr<Shader::Program> _ssrApplyShader;

    std::shared_ptr<Framebuffer> _ssaoBuffer;
    std::shared_ptr<Shader::Program> _ssaoShader;

    std::shared_ptr<Framebuffer> _hzbBuffer;
    std::shared_ptr<Shader::Program> _hzbShader;

    std::shared_ptr<Shader::Program> _compositingShader;

    std::shared_ptr<Shader::Program> _deferredMaterialShader;

    std::shared_ptr<Texture2D> _defaultBRDF;
    std::shared_ptr<Geometry> _displayQuad;

    std::weak_ptr<Window> _window;

    Context _context;
    uint32_t _frameNbr{ 0 };
    FrameRenderer& _frameRenderer;
    double _lastTicks{ 0 };
    double _deltaTime{ 0 };
};
};