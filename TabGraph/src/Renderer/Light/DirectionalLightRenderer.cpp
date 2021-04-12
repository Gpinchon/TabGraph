/*
* @Author: gpinchon
* @Date:   2021-04-10 15:36:06
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 14:52:34
*/

#include "Renderer/Light/DirectionalLightRenderer.hpp"
#include "Light/DirectionalLight.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Light/DirectionalLightRenderer.hpp"
//#endif

namespace Renderer {
DirectionalLightRenderer::DirectionalLightRenderer(DirectionalLight& light)
    : LightRenderer(light)
{
    _impl.reset(new DirectionalLightRenderer::Impl);
}
};
