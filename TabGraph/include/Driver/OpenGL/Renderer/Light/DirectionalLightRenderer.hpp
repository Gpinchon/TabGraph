/*
* @Author: gpinchon
* @Date:   2021-04-10 15:28:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-10 15:28:08
*/
#pragma once

#include <Renderer/Light/LightRenderer.hpp>
#include <Camera/Camera.hpp>

#include <glm/glm.hpp>

class Framebuffer;
class DirectionalLight;
namespace Shader {
class Program;
}

namespace Renderer {
struct Options;
class DirectionalLightRenderer : public LightRenderer {
public:
    DirectionalLightRenderer(DirectionalLight&);
    virtual void Render(const Renderer::Options&) override;
    virtual void UpdateLightProbe(const Renderer::Options& options, LightProbe&) override;

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

glm::mat4 DirectionalLightShadowViewMatrix(DirectionalLight&);
Camera::Projection DirectionalLightShadowProjectionInfinite(DirectionalLight&, const Renderer::Options&);
Camera::Projection DirectionalLightShadowProjectionFinite(DirectionalLight&);
