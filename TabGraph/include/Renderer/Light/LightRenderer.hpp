/*
* @Author: gpinchon
* @Date:   2021-04-10 15:27:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 14:51:24
*/
#pragma once

//#include "Light/Light.hpp"
#include "Renderer/Renderer.hpp"
#include <memory>

class Light;
class LightProbe;

namespace Renderer {
void Render(std::shared_ptr<Light>, const Renderer::Options&);
void UpdateLightProbe(std::shared_ptr<Light>, LightProbe&);
class LightRenderer {
public:
    class Impl;
    struct ImplDeleter {
        void operator()(LightRenderer::Impl*);
    };
    LightRenderer(Light&);
    void Render(const Renderer::Options&);
    void UpdateLightProbe(LightProbe&);

protected:
    LightRenderer::Impl& GetImpl();
    std::unique_ptr<LightRenderer::Impl, LightRenderer::ImplDeleter> _impl;
    Light& _light;
};
};
