/*
* @Author: gpinchon
* @Date:   2021-04-11 14:45:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 15:39:15
*/
#pragma once

#include "Renderer/Light/LightRenderer.hpp"

#include <glm/glm.hpp>
#include <vector>

class HDRLight;
namespace Shader {
class Program;
}

namespace Renderer {
class HDRLightRenderer : public LightRenderer {
public:
    HDRLightRenderer(HDRLight&);
    void FlagDirty();
    virtual void Render(const Renderer::Options&) override;
    virtual void UpdateLightProbe(const Renderer::Options&, LightProbe&) override;

protected:
    void _RenderDeferredLighting(HDRLight&, const Renderer::Options&);
    void _Update(HDRLight&);
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    bool _dirty{ true };
};
};