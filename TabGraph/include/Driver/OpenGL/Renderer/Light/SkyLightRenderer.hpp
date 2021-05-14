/*
* @Author: gpinchon
* @Date:   2021-04-11 16:24:29
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:26
*/
#pragma once

#include "Renderer/Light/LightRenderer.hpp"

#include <glm/glm.hpp>
#include <vector>

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
    void FlagDirty();
    virtual void Render(const Renderer::Options&) override;
    virtual void UpdateLightProbe(LightProbe&) override;

protected:
    void _RenderDeferredLighting(SkyLight&, const Renderer::Options&);
    void _RenderShadow(SkyLight&, const Renderer::Options&);
    void _UpdateLUT(SkyLight&);
    std::shared_ptr<Framebuffer> _shadowBuffer;
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<TextureCubemap> _reflectionLUT;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Framebuffer> _diffuseLUTBuffer;
    bool _dirty { true };
};
};