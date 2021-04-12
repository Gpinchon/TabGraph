/*
* @Author: gpinchon
* @Date:   2021-04-11 16:24:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 16:28:03
*/
#pragma once

#include "Renderer/Light/LightRenderer.hpp"

class SkyLight;

namespace Renderer {
class SkyLightRenderer : public LightRenderer {
public:
    SkyLightRenderer(SkyLight&);
    void FlagDirty();

private:
    class Impl;
    friend Impl;
};
};