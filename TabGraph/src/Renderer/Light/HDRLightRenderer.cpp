/*
* @Author: gpinchon
* @Date:   2021-04-11 14:45:35
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 14:45:35
*/
#include "Renderer/Light/HDRLightRenderer.hpp"
#include "Light/HDRLight.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/HDRLightRenderer.hpp"
//#endif

namespace Renderer {
HDRLightRenderer::HDRLightRenderer(HDRLight& light)
	: LightRenderer(light)
{
	_impl.reset(new HDRLightRenderer::Impl);
}
void HDRLightRenderer::FlagDirty()
{
	static_cast<HDRLightRenderer::Impl&>(GetImpl()).FlagDirty();
}
};
