/*
* @Author: gpinchon
* @Date:   2021-03-22 18:30:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-22 18:53:12
*/

#include "Renderer/Renderer.hpp"
#include "Renderer/GeometryRenderer.hpp"
#include "Renderer/SceneRenderer.hpp"
#include "Camera/Camera.hpp" // for Camera
#include "Config.hpp" // for Config
#include "Engine.hpp" // for UpdateMutex, SwapInterval
#include "Skybox.hpp" // for Skybox
#include "Framebuffer.hpp" // for Framebuffer
#include "Light/Light.hpp" // for Light, Directionnal, Point
#include "Mesh/Geometry.hpp" // for Geometry
#include "Scene/Scene.hpp"
#include "Skybox.hpp"
#include "Shader/Global.hpp" // for Shader::Global
#include "Shader/Program.hpp" // for Shader::Program
#include "Texture/Cubemap.hpp"
#include "Texture/Texture2D.hpp" // for Texture2D
#include "Window.hpp" // for Window
#include "Debug.hpp"

//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/Renderer.hpp"
//#endif

#include <algorithm> // for max, remove_if
#include <atomic> // for atomic
#include <glm/glm.hpp> // for glm::inverse, vec2_scale, vec3_scale
#include <iostream> // for char_traits, endl, cout, operator<<, ost...
#include <mutex> // for mutex
#include <stdint.h> // for uint64_t, uint16_t
#include <string> // for operator+, to_string, string
#include <thread> // for thread
#include <vector> // for vector<>::iterator, vector

// quad is a singleton
const std::shared_ptr<Geometry> Renderer::DisplayQuad()
{
    return GetImpl()->DisplayQuad();
}

std::shared_ptr<Framebuffer> Renderer::DeferredGeometryBuffer()
{
    return GetImpl()->DeferredGeometryBuffer();
}

std::shared_ptr<Framebuffer> Renderer::DeferredLightingBuffer()
{
    return GetImpl()->DeferredLightingBuffer();
}

std::shared_ptr<Framebuffer> Renderer::ForwardTransparentRenderBuffer()
{
    return GetImpl()->ForwardTransparentRenderBuffer();
}

std::shared_ptr<Framebuffer> Renderer::OpaqueRenderBuffer()
{
    return GetImpl()->OpaqueRenderBuffer();
}

std::shared_ptr<Framebuffer> Renderer::FinalRenderBuffer()
{
    return GetImpl()->FinalRenderBuffer();
}

std::shared_ptr<Framebuffer> Renderer::PreviousRenderBuffer()
{
    return GetImpl()->PreviousRenderBuffer();
}

void Renderer::Init()
{
    GetImpl()->Init();
}

Renderer::Context &Renderer::GetContext()
{
    return GetImpl()->GetContext();
}

//Signal<uint32_t, float>& Renderer::OnFrameBegin()
//{
//    return GetImpl()->OnFrameBegin;
//}
//
//Signal<uint32_t, float>& Renderer::OnFrameEnd()
//{
//    return GetImpl()->OnFrameEnd;
//}

void Renderer::RenderFrame()
{
    GetImpl()->RenderFrame();
}

uint32_t Renderer::FrameNumber()
{
    return GetImpl()->FrameNumber();
}
