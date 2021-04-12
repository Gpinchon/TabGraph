/*
* @Author: gpinchon
* @Date:   2021-03-26 11:48:39
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 20:51:29
*/
#pragma once

#include "Renderer/Light/LightRenderer.hpp"

class DirectionalLight;

namespace Renderer {
class DirectionalLightRenderer : public LightRenderer {
public:
    DirectionalLightRenderer(DirectionalLight&);

private:
    class Impl;
    friend Impl;
};
};
