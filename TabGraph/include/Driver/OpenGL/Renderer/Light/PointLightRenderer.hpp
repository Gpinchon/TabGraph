/*
* @Author: gpinchon
* @Date:   2021-04-11 20:51:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 20:52:16
*/
#pragma once

#include "Renderer/Light/PointLightRenderer.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/LightRenderer.hpp"
//#endif

#include <glm/glm.hpp>

glm::mat4 PointLightShadowViewMatrix(PointLight&);
glm::mat4 PointLightShadowProjectionMatrixInfinite(PointLight&, const Renderer::Options&);
glm::mat4 PointLightShadowProjectionMatrixFinite(PointLight&);

namespace Renderer {
class PointLightRenderer::Impl : public LightRenderer::Impl {
public:
    Impl();
    virtual void Render(Light&, const Renderer::Options&) override;
    virtual void UpdateLightProbe(Light&, LightProbe&) override;

protected:
    void _RenderDeferredLighting(PointLight&, const Renderer::Options&);
    void _RenderShadow(PointLight&, const Renderer::Options&);
    void _RenderShadowInfinite(PointLight&, const Renderer::Options&);
    void _RenderShadowFinite(PointLight&, const Renderer::Options&);
    std::shared_ptr<Shader::Program> _lightingShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Framebuffer> _shadowBuffer;
};
};