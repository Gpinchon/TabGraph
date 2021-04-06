/*
* @Author: gpinchon
* @Date:   2021-03-22 20:34:39
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 08:02:05
*/

#include "Renderer/MeshRenderer.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/MeshRenderer.hpp"
//#endif
#include "Mesh/Mesh.hpp"

namespace Renderer {
void OnFrameBegin(std::shared_ptr<Mesh> mesh, uint32_t frameNbr, float delta)
{
    mesh->GetRenderer().OnFrameBegin(frameNbr, delta);
}

void Render(std::shared_ptr<Mesh> mesh, const Options& options)
{
    mesh->GetRenderer().Render(options, glm::mat4(1), glm::mat4(1));
}

void Render(std::shared_ptr<Mesh> mesh, const Options& options, const glm::mat4& parentTransform)
{
    mesh->GetRenderer().Render(options, parentTransform, parentTransform);
}

void Render(std::shared_ptr<Mesh> mesh, const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform)
{
    mesh->GetRenderer().Render(options, parentTransform, parentLastTransform);
}

void OnFrameEnd(std::shared_ptr<Mesh> mesh, uint32_t frameNbr, float delta)
{
    mesh->GetRenderer().OnFrameEnd(frameNbr, delta);
}

MeshRenderer::~MeshRenderer()
{
}

MeshRenderer::MeshRenderer(Mesh& mesh)
    : _mesh(mesh)
    , _impl(new MeshRenderer::Impl)
{
}

void MeshRenderer::OnFrameBegin(uint32_t frameNbr, float delta)
{
    _impl->OnFrameBegin(_mesh, frameNbr, delta);
}

void MeshRenderer::Render(const ::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform)
{
    _impl->Render(_mesh, options, parentTransform, parentLastTransform);
}

void MeshRenderer::OnFrameEnd(uint32_t frameNbr, float delta)
{
    _impl->OnFrameEnd(_mesh, frameNbr, delta);
}

MeshRenderer::Impl& MeshRenderer::GetImpl()
{
    return *_impl;
}
};
