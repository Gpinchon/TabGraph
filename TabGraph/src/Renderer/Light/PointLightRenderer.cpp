/*
* @Author: gpinchon
* @Date:   2021-04-11 20:53:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 20:53:41
*/
#include "Renderer/Light/PointLightRenderer.hpp"
#include "Light/PointLight.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/PointLightRenderer.hpp"
//#endif

namespace Renderer {
PointLightRenderer::PointLightRenderer(PointLight& light)
	: LightRenderer(light)
{
	_impl.reset(new Renderer::PointLightRenderer::Impl);
}
}