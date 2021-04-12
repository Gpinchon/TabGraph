/*
* @Author: gpinchon
* @Date:   2021-03-22 20:32:34
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:54:56
*/
#pragma once
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/MeshRenderer.hpp"
//#endif

#include <glm/fwd.hpp>
#include <memory>

class Mesh;
namespace Renderer {
struct Options;
void OnFrameBegin(std::shared_ptr<Mesh> mesh, uint32_t frameNbr, float delta);
void Render(std::shared_ptr<Mesh> mesh, const ::Renderer::Options& options);
void Render(std::shared_ptr<Mesh> mesh, const ::Renderer::Options& options, const glm::mat4& parentTransform);
void Render(std::shared_ptr<Mesh> mesh, const ::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform);
void OnFrameEnd(std::shared_ptr<Mesh> mesh, uint32_t frameNbr, float delta);
};