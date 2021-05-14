/*
* @Author: gpinchon
* @Date:   2021-03-25 11:04:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:55:36
*/
#pragma once

//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/SceneRenderer.hpp"
//#endif

#include <glm/fwd.hpp>
#include <memory>

class Scene;
class LightProbe;

namespace Renderer {
struct Options;
void OnFrameBegin(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta);
void Render(std::shared_ptr<Scene> scene, const Renderer::Options& options);
void Render(std::shared_ptr<Scene> scene, const Renderer::Options& options, const glm::mat4& rootMatrix);
void OnFrameEnd(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta);
};
