/*
* @Author: gpinchon
* @Date:   2021-04-11 14:45:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 15:39:15
*/
#pragma once

#include "Renderer/Light/HDRLightRenderer.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/LightRenderer.hpp"
//#endif

#include <glm/glm.hpp>
#include <vector>

class HDRLight;

namespace Renderer {
class HDRLightRenderer::Impl : public LightRenderer::Impl {
public:
    Impl();
    void FlagDirty();
    virtual void Render(Light&, const Renderer::Options&) override;
    virtual void UpdateLightProbe(Light&, LightProbe&) override;

protected:
    void _RenderDeferredLighting(HDRLight&, const Renderer::Options&);
    void _Update(HDRLight&);
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    bool _dirty{ true };
};
};