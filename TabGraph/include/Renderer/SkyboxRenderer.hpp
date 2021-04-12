/*
* @Author: gpinchon
* @Date:   2021-03-24 16:05:51
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:49:42
*/
#pragma once

//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/SkyboxRenderer.hpp"
//#endif

#include <memory>

class Skybox;

namespace Renderer {
struct Options;
void OnFrameBegin(std::shared_ptr<Skybox> skybox, uint32_t frameNbr, float delta);
void Render(std::shared_ptr<Skybox> skybox, const Renderer::Options& options);
void OnFrameEnd(std::shared_ptr<Skybox> skybox, uint32_t frameNbr, float delta);
};