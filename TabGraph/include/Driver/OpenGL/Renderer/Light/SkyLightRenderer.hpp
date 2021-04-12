/*
* @Author: gpinchon
* @Date:   2021-04-11 16:24:29
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 16:32:35
*/
#pragma once

#include "Renderer/Light/SkyLightRenderer.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/LightRenderer.hpp"
//#endif

#include <glm/glm.hpp>
#include <vector>

class SkyLight;
class Cubemap;

namespace Renderer {
class SkyLightRenderer::Impl : public LightRenderer::Impl {
public:
    Impl();
    void FlagDirty();
    virtual void Render(Light&, const Renderer::Options&) override;
    virtual void UpdateLightProbe(Light&, LightProbe&) override;

protected:
    void _RenderDeferredLighting(SkyLight&, const Renderer::Options&);
    void _RenderShadow(SkyLight&, const Renderer::Options&);
    void _UpdateLUT(SkyLight&);
    std::shared_ptr<Framebuffer> _shadowBuffer;
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<Cubemap> _reflectionLUT;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    bool _dirty { true };
};
};