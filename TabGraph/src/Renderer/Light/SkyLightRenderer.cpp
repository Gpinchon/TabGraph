/*
* @Author: gpinchon
* @Date:   2021-04-11 16:24:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 16:34:56
*/
#include "Renderer/Light/SkyLightRenderer.hpp"
#include "Light/SkyLight.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/SkyLightRenderer.hpp"
//#endif

namespace Renderer {
SkyLightRenderer::SkyLightRenderer(SkyLight& light)
	: LightRenderer(light)
{
	_impl.reset(new SkyLightRenderer::Impl);
}
void SkyLightRenderer::FlagDirty()
{
	static_cast<SkyLightRenderer::Impl&>(GetImpl()).FlagDirty();
}
}