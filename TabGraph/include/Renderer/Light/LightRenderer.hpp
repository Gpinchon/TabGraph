/*
* @Author: gpinchon
* @Date:   2021-04-10 15:27:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-12 22:01:09
*/
#pragma once

#include <memory>

class Light;
class LightProbe;

namespace Renderer {
struct Options;
void Render(std::shared_ptr<Light>, const Renderer::Options&);
void UpdateLightProbe(std::shared_ptr<Light>, const Renderer::Options&, LightProbe&);
class LightRenderer {
public:
    LightRenderer(Light&);
    virtual void Render(const Renderer::Options&) = 0;
    virtual void UpdateLightProbe(const Renderer::Options&, LightProbe&) = 0;

protected:
    Light& _light;
};
};
