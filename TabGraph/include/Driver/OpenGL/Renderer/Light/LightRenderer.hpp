/*
* @Author: gpinchon
* @Date:   2021-04-10 15:27:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-10 15:27:52
*/
#pragma once

#include "Renderer/Light/LightRenderer.hpp"

class Light;

namespace Renderer {
	class LightRenderer::Impl {
	public:
		virtual void Render(Light&, const Renderer::Options&) = 0;
		virtual void UpdateLightProbe(Light&, LightProbe&) = 0;
	};
}
