/*
* @Author: gpinchon
* @Date:   2021-03-26 14:20:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-02 19:11:43
*/

#pragma once
#include "Component.hpp"
#include "Event/Signal.hpp"

#include <memory>

class Texture2D;
class Framebuffer;
class Geometry;
namespace Shader {
class Program;
};

namespace Renderer {
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
class Impl : public Component {
public:
    Impl();
    void Init();
    Context& GetContext();
    static void RenderFrame();

    static std::shared_ptr<Framebuffer> DeferredGeometryBuffer();
    static std::shared_ptr<Framebuffer> DeferredLightingBuffer();
    static std::shared_ptr<Framebuffer> ForwardTransparentRenderBuffer();
    static std::shared_ptr<Framebuffer> OpaqueRenderBuffer();
    static std::shared_ptr<Framebuffer> FinalRenderBuffer();
    static std::shared_ptr<Framebuffer> PreviousRenderBuffer();
    static std::shared_ptr<Texture2D> DefaultBRDFLUT();

    static const std::shared_ptr<Geometry> DisplayQuad();
    static const uint32_t FrameNumber();
    virtual std::shared_ptr<Component> _Clone() override;

private:
    void _RenderFrame();

    std::shared_ptr<Framebuffer> _deferredLightingBuffer;
    std::shared_ptr<Framebuffer> _deferredRenderBuffer;
    std::shared_ptr<Framebuffer> _forwardTransparentRenderBuffer;
    std::shared_ptr<Framebuffer> _opaqueRenderBuffer;
    std::shared_ptr<Framebuffer> _finalRenderBuffer;
    std::shared_ptr<Framebuffer> _previousRenderBuffer;

    void _OpaquePass();
    void _HZBPass();
    void _SSRPass();
    void _SSAOPass();
    void _DeferredMaterialPass();
    void _TransparentPass();
    void _CompositingPass();
    std::shared_ptr<Texture2D> _DefaultBRDFLUT();

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

    Context _context;
    uint32_t _frameNbr{ 0 };
};
std::shared_ptr<Renderer::Impl> GetImpl();
};