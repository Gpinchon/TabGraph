/*
* @Author: gpinchon
* @Date:   2021-04-11 20:50:51
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 20:51:25
*/
#pragma once

#include "Renderer/Light/LightRenderer.hpp"

class PointLight;

namespace Renderer {
class PointLightRenderer : public LightRenderer {
public:
    PointLightRenderer(PointLight&);

private:
    class Impl;
    friend Impl;
};
};