/*
* @Author: gpinchon
* @Date:   2021-04-10 15:28:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-10 15:28:08
*/
#pragma once

#include "Renderer/Light/DirectionalLightRenderer.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/LightRenderer.hpp"
//#endif

#include <glm/glm.hpp>

glm::mat4 DirectionalLightShadowViewMatrix(DirectionalLight&);
glm::mat4 DirectionalLightShadowProjectionMatrixInfinite(DirectionalLight&, const Renderer::Options&);
glm::mat4 DirectionalLightShadowProjectionMatrixFinite(DirectionalLight&);

namespace Renderer {
class DirectionalLightRenderer::Impl : public LightRenderer::Impl {
public:
    Impl();
    virtual void Render(Light&, const Renderer::Options&) override;
    virtual void UpdateLightProbe(Light&, LightProbe&) override;

protected:
    void _RenderDeferredLighting(DirectionalLight&, const Renderer::Options&);
    void _RenderShadow(DirectionalLight&, const Renderer::Options&);
    void _RenderShadowInfinite(DirectionalLight&, const Renderer::Options&);
    void _RenderShadowFinite(DirectionalLight&, const Renderer::Options&);
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Framebuffer> _shadowBuffer;
};
};
