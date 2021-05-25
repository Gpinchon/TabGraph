/*
* @Author: gpinchon
* @Date:   2021-03-25 11:04:12
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 08:05:14
*/

#include "Renderer/SceneRenderer.hpp"
#include "Scene/Scene.hpp"

//#if RENDERINGAPI == OpenGL
#include "Driver/OpenGL/Renderer/SceneRenderer.hpp"
//#endif

#include <glm/glm.hpp>

namespace Renderer {
void OnFrameBegin(std::shared_ptr<Scene> scene, const Options& options)
{
    scene->GetRenderer().OnFrameBegin(options);
}

void Render(std::shared_ptr<Scene> scene, const Options& options)
{
    scene->GetRenderer().Render(options, glm::mat4(1));
}

void Render(std::shared_ptr<Scene> scene, const Options& options, const glm::mat4& rootMatrix)
{
    scene->GetRenderer().Render(options, rootMatrix);
}

void OnFrameEnd(std::shared_ptr<Scene> scene, const Options& options)
{
    scene->GetRenderer().OnFrameEnd(options);
}
};
