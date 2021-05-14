/*
* @Author: gpinchon
* @Date:   2021-03-25 11:04:12
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 08:05:14
*/

#include "Renderer/SceneRenderer.hpp"
#include "Scene/Scene.hpp"

//#ifdef OPENGL
#include "Driver/OpenGL/Renderer/SceneRenderer.hpp"
//#endif

#include <glm/glm.hpp>

namespace Renderer {
void OnFrameBegin(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta)
{
    scene->GetRenderer().OnFrameBegin(frameNbr, delta);
}

void Render(std::shared_ptr<Scene> scene, const Options& options)
{
    scene->GetRenderer().Render(options, glm::mat4(1));
}

void OnFrameEnd(std::shared_ptr<Scene> scene, uint32_t frameNbr, float delta)
{
    scene->GetRenderer().OnFrameEnd(frameNbr, delta);
}
};
