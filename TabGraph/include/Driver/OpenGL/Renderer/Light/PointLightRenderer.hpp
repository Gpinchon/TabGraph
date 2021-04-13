/*
* @Author: gpinchon
* @Date:   2021-04-11 20:51:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 20:52:16
*/
#pragma once

#include "Renderer/Light/LightRenderer.hpp"

#include <glm/glm.hpp>

class Framebuffer;
class PointLight;
namespace Shader {
class Program;
};

namespace Renderer {
class PointLightRenderer : public LightRenderer {
public:
    PointLightRenderer(PointLight &);
    virtual void Render(const Renderer::Options&) override;
    virtual void UpdateLightProbe(LightProbe&) override;

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

glm::mat4 PointLightShadowViewMatrix(PointLight&);
glm::mat4 PointLightShadowProjectionMatrixInfinite(PointLight&, const Renderer::Options&);
glm::mat4 PointLightShadowProjectionMatrixFinite(PointLight&);
