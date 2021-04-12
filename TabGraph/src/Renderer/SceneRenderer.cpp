/*
* @Author: gpinchon
* @Date:   2021-03-25 11:04:12
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 08:05:14
*/

//#include "Renderer/SceneRenderer.hpp"
//#include "Scene/Scene.hpp"
//
//#include <memory>
//
//namespace Renderer {
//void OnFrameBegin(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta)
//{
//    scene->GetRenderer().OnFrameBegin(frameNbr, delta);
//}
//void Render(std::shared_ptr<Scene> scene, const Renderer::Options& options)
//{
//    scene->GetRenderer().Render(options, glm::mat4(1));
//}
//void OnFrameEnd(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta)
//{
//    scene->GetRenderer().OnFrameEnd(frameNbr, delta);
//}
//}


#include "Renderer/SceneRenderer.hpp"
//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/SceneRenderer.hpp"
//#endif
#include "Scene/Scene.hpp"

#include <glm/glm.hpp>

namespace Renderer {
void OnFrameBegin(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta)
{
    scene->GetRenderer().OnFrameBegin(frameNbr, delta);
}

void Render(std::shared_ptr<Scene> scene, const Options& options)
{
    scene->GetRenderer().Render(options);
}

void OnFrameEnd(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta)
{
    scene->GetRenderer().OnFrameEnd(frameNbr, delta);
}

SceneRenderer::SceneRenderer(Scene& scene)
    : _scene(scene)
    , _impl(std::make_unique<SceneRenderer::Impl>())
{
}

SceneRenderer::~SceneRenderer()
{
}

SceneRenderer::Impl& SceneRenderer::GetImpl()
{
    return *_impl;
}

LightProbe& SceneRenderer::GetClosestLightProbe(const glm::vec3& position)
{
    return GetImpl().GetClosestLightProbe(position);
}

void SceneRenderer::OnFrameBegin(uint32_t frameNbr, float delta)
{
    GetImpl().OnFrameBegin(_scene, frameNbr, delta);
}

void SceneRenderer::Render(const Options& options)
{
    Render(options, glm::mat4(1));
}

void SceneRenderer::Render(const Options& options, const glm::mat4& rootMatrix)
{
    GetImpl().Render(_scene, options, rootMatrix);
}

void SceneRenderer::OnFrameEnd(uint32_t frameNbr, float delta)
{
    GetImpl().OnFrameEnd(_scene, frameNbr, delta);
}
};
