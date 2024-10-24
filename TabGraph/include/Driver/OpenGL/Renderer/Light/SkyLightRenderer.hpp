/*
* @Author: gpinchon
* @Date:   2021-04-11 16:24:29
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 18:52:37
*/
#pragma once

#include <Renderer/Light/LightRenderer.hpp>

#include <glm/glm.hpp>
#include <vector>

class Geometry;
class SkyLight;
class TextureCubemap;
class Framebuffer;
namespace Shader {
class Program;
};

namespace Renderer {
class SkyLightRenderer : public LightRenderer {
public:
    SkyLightRenderer(SkyLight&);
    virtual void Render(const Renderer::Options&) override;
    virtual void UpdateLightProbe(const Renderer::Options&, LightProbe&) override;

protected:
    void _RenderDeferredLighting(SkyLight&, const Renderer::Options&);
    void _RenderShadow(SkyLight&, const Renderer::Options&);
    void _UpdateLUT(SkyLight&, const Renderer::Options&);
    std::shared_ptr<Framebuffer> _shadowBuffer;
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<TextureCubemap> _reflectionLUT;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Framebuffer> _diffuseLUTBuffer;
    std::shared_ptr<Geometry> _deferredGeometry;
};
};