/*
* @Author: gpinchon
* @Date:   2021-04-11 14:45:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 16:27:52
*/
#pragma once

#include "Renderer/Light/LightRenderer.hpp"

class HDRLight;

namespace Renderer {
class HDRLightRenderer : public LightRenderer {
public:
    HDRLightRenderer(HDRLight&);
    void FlagDirty();

private:
    class Impl;
    friend Impl;
};
};